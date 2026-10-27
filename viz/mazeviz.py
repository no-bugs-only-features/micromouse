"""
Micromouse orchestrator: ctypes bridge to the C maze engine and swappable mouse
libraries, a Python-side simulator, and a pywebview shell for the visualizer UI.

C contracts this file assumes
-----------------------------
libmazegym (maze engine):
    typedef struct {
        uint8_t walls[16][16];
        uint8_t distances[16][16];   // 255 = unreachable
        uint8_t known[16][16];
    } Maze;
    Maze *generate_maze(int size, uint32_t seed, int braid_pct);  // NULL if no solvable maze
    void  compute_distances(Maze *m, int size, int goal_x, int goal_y);
    void  free_maze(Maze *m);

Mouse libraries (.so / .dll / .dylib), one per algorithm:
    void algo_init(int maze_size);
    int  algo_step(uint8_t sensors);   // bit0 front wall, bit1 left, bit2 right (1 = wall)
                                       // returns 0 forward, 1 left, 2 right, 3 back

Conventions (match maze_json_format.md): wall bits N=8, E=4, S=2, W=1;
(0,0) is the top-left cell and y grows southward.
"""
from __future__ import annotations

import ctypes
import functools
import json
import math
import random
import sys
import threading
from collections import deque
from dataclasses import dataclass
from pathlib import Path

import webview  # pip install pywebview

MAX_SIZE = 16
NORTH, EAST, SOUTH, WEST = 8, 4, 2, 1
DIR_BITS = (NORTH, EAST, SOUTH, WEST)  # clockwise from north
DIR_NAMES = "NESW"
DX = (0, 1, 0, -1)
DY = (-1, 0, 1, 0)
UNREACHABLE = 255
FORWARD, LEFT, RIGHT, BACK = range(4)
TURN_DELTA = {FORWARD: 0, LEFT: 3, RIGHT: 1, BACK: 2}  # heading change, clockwise quarters
TURN_QUARTERS = {FORWARD: 0, LEFT: 1, RIGHT: 1, BACK: 2}


# --------------------------------------------------------------------------
# Maze data and the C engine bridge
# --------------------------------------------------------------------------
class CMaze(ctypes.Structure):
    _fields_ = [
        ("walls", (ctypes.c_uint8 * MAX_SIZE) * MAX_SIZE),
        ("distances", (ctypes.c_uint8 * MAX_SIZE) * MAX_SIZE),
        ("known", (ctypes.c_uint8 * MAX_SIZE) * MAX_SIZE),
    ]


@dataclass
class MazeData:
    """Plain-Python copy of a maze. The C allocation is freed right after copying."""
    size: int
    walls: list
    start: tuple
    goal: tuple
    distances: list  # -1 = unreachable

    def to_json(self) -> dict:
        return {
            "size": self.size,
            "walls": self.walls,
            "start": {"x": self.start[0], "y": self.start[1]},
            "goal": {"x": self.goal[0], "y": self.goal[1]},
            "distances": self.distances,
        }


def bfs_distances(walls, size, goal):
    """Pure-Python oracle, used for mazes loaded from JSON (the C oracle needs a C Maze)."""
    dist = [[-1] * size for _ in range(size)]
    gx, gy = goal
    dist[gy][gx] = 0
    queue = deque([goal])
    while queue:
        x, y = queue.popleft()
        for d in range(4):
            if walls[y][x] & DIR_BITS[d]:
                continue
            nx, ny = x + DX[d], y + DY[d]
            if 0 <= nx < size and 0 <= ny < size and dist[ny][nx] == -1:
                dist[ny][nx] = dist[y][x] + 1
                queue.append((nx, ny))
    return dist


def check_wall_consistency(walls, size):
    for y in range(size):
        for x in range(size):
            if x + 1 < size and bool(walls[y][x] & EAST) != bool(walls[y][x + 1] & WEST):
                raise ValueError(f"inconsistent wall between ({x},{y}) and ({x + 1},{y})")
            if y + 1 < size and bool(walls[y][x] & SOUTH) != bool(walls[y + 1][x] & NORTH):
                raise ValueError(f"inconsistent wall between ({x},{y}) and ({x},{y + 1})")


def find_lib(folder: Path, stem: str) -> Path:
    suffix = {"win32": ".dll", "darwin": ".dylib"}.get(sys.platform, ".so")
    for name in (f"lib{stem}{suffix}", f"{stem}{suffix}"):
        if (folder / name).exists():
            return folder / name
    raise FileNotFoundError(f"{stem}{suffix} not found in {folder}; build the C engine first")


class Engine:
    def __init__(self, lib_path: Path):
        lib = ctypes.CDLL(str(lib_path))
        lib.generate_maze.argtypes = [ctypes.c_int, ctypes.c_uint32, ctypes.c_int]
        lib.generate_maze.restype = ctypes.POINTER(CMaze)
        lib.compute_distances.argtypes = [ctypes.POINTER(CMaze), ctypes.c_int, ctypes.c_int, ctypes.c_int]
        lib.compute_distances.restype = None
        lib.free_maze.argtypes = [ctypes.POINTER(CMaze)]
        lib.free_maze.restype = None
        self.lib = lib

    def generate(self, size: int, seed: int, braid_pct: int) -> MazeData:
        if not 2 <= size <= MAX_SIZE:
            raise ValueError(f"size must be 2..{MAX_SIZE}")
        ptr = self.lib.generate_maze(size, seed, braid_pct)
        if not ptr:
            raise RuntimeError("engine could not produce a solvable maze for these parameters")
        goal = (size // 2, size // 2)
        try:  # copy everything out, then free: Python never holds a C pointer past this block
            self.lib.compute_distances(ptr, size, *goal)
            m = ptr.contents
            walls = [[m.walls[y][x] for x in range(size)] for y in range(size)]
            dist = [[-1 if m.distances[y][x] == UNREACHABLE else m.distances[y][x]
                     for x in range(size)] for y in range(size)]
        finally:
            self.lib.free_maze(ptr)
        return MazeData(size, walls, (0, 0), goal, dist)


def load_maze_json(path: str) -> MazeData:
    data = json.loads(Path(path).read_text())
    size, walls = int(data["size"]), data["walls"]
    if not 2 <= size <= MAX_SIZE:
        raise ValueError(f"size must be 2..{MAX_SIZE}")
    if len(walls) != size or any(len(row) != size for row in walls):
        raise ValueError("walls must be a size x size grid")
    check_wall_consistency(walls, size)
    start = (int(data["start"]["x"]), int(data["start"]["y"]))
    goal = (int(data["goal"]["x"]), int(data["goal"]["y"]))
    for x, y in (start, goal):
        if not (0 <= x < size and 0 <= y < size):
            raise ValueError("start/goal outside the maze")
    dist = bfs_distances(walls, size, goal)  # always recomputed: trust the oracle over the file
    if dist[start[1]][start[0]] < 0:
        raise ValueError("goal is unreachable from start")
    return MazeData(size, walls, start, goal, dist)


# --------------------------------------------------------------------------
# Mouse library, spec, and simulator
# --------------------------------------------------------------------------
class Mouse:
    def __init__(self, path: str):
        self.name = Path(path).name
        lib = ctypes.CDLL(str(path))
        lib.algo_init.argtypes = [ctypes.c_int]
        lib.algo_init.restype = None
        lib.algo_step.argtypes = [ctypes.c_uint8]
        lib.algo_step.restype = ctypes.c_int
        self.lib = lib

    def init(self, size: int):
        self.lib.algo_init(size)

    def step(self, sensors: int) -> int:
        return self.lib.algo_step(sensors)


@dataclass
class MouseSpec:
    max_speed: float = 3.0   # m/s
    accel: float = 12.0      # m/s^2 (used for accel and decel)
    turn_speed: float = 0.9  # m/s, speed through and between turns
    cell_size: float = 0.18  # m

    @classmethod
    def from_dict(cls, d: dict) -> "MouseSpec":
        spec = cls(**{k: float(d[k]) for k in ("max_speed", "accel", "turn_speed", "cell_size") if k in d})
        if min(spec.max_speed, spec.accel, spec.turn_speed, spec.cell_size) <= 0:
            raise ValueError("mouse spec values must be positive")
        if spec.turn_speed > spec.max_speed:
            raise ValueError("turn speed cannot exceed max speed")
        return spec


@dataclass
class Step:
    x: int
    y: int
    heading: int
    turns: int  # quarter turns made before this move (0 = straight)


class SimError(Exception):
    pass


def sensors_at(walls_cell: int, heading: int) -> int:
    front = bool(walls_cell & DIR_BITS[heading])
    left = bool(walls_cell & DIR_BITS[(heading + 3) % 4])
    right = bool(walls_cell & DIR_BITS[(heading + 1) % 4])
    return front | (left << 1) | (right << 2)


def simulate(maze: MazeData, mouse: Mouse, max_steps: int | None = None) -> list:
    max_steps = max_steps or maze.size ** 2 * 8
    mouse.init(maze.size)
    x, y = maze.start
    heading = next((d for d in range(4) if not maze.walls[y][x] & DIR_BITS[d]), 0)
    trace = [Step(x, y, heading, 0)]
    for _ in range(max_steps):
        if (x, y) == maze.goal:
            return trace
        move = mouse.step(sensors_at(maze.walls[y][x], heading))
        if move not in TURN_DELTA:
            raise SimError(f"mouse returned invalid move {move}")
        heading = (heading + TURN_DELTA[move]) % 4
        if maze.walls[y][x] & DIR_BITS[heading]:
            raise SimError(f"mouse drove into a wall at ({x},{y}) heading {DIR_NAMES[heading]}")
        x, y = x + DX[heading], y + DY[heading]
        trace.append(Step(x, y, heading, TURN_QUARTERS[move]))
    if (x, y) == maze.goal:
        return trace
    raise SimError(f"step limit ({max_steps}) reached before the goal")


# --------------------------------------------------------------------------
# Kinematics: turn a cell trace into timestamps
# --------------------------------------------------------------------------
def _t_acc(s, v0, a):
    """Time to cover distance s accelerating from v0 at a."""
    return (-v0 + math.sqrt(v0 * v0 + 2 * a * s)) / a


def run_time_at(s, total, v0, vmax, a):
    """Time at distance s along a straight run: accelerate, cruise, decelerate (symmetric)."""
    da = (vmax ** 2 - v0 ** 2) / (2 * a)
    if total >= 2 * da:  # trapezoidal profile
        ta = (vmax - v0) / a
        if s <= da:
            return _t_acc(s, v0, a)
        if s >= total - da:
            return 2 * ta + (total - 2 * da) / vmax - _t_acc(total - s, v0, a)
        return ta + (s - da) / vmax
    half = total / 2  # triangular profile: never reaches vmax
    if s <= half:
        return _t_acc(s, v0, a)
    return 2 * _t_acc(half, v0, a) - _t_acc(total - s, v0, a)


def build_run_log(trace: list, spec: MouseSpec) -> list:
    """The mouse stops at turn speed between runs; straight runs get a full velocity profile."""
    def entry(t, x, y, h):
        return {"t": round(t, 4), "x": x, "y": y, "heading": DIR_NAMES[h]}

    quarter_turn_time = (math.pi * spec.cell_size / 4) / spec.turn_speed  # arc of radius cell/2
    log = [entry(0.0, trace[0].x, trace[0].y, trace[0].heading)]
    clock, i = 0.0, 1
    while i < len(trace):
        if trace[i].turns:
            clock += trace[i].turns * quarter_turn_time
            prev = trace[i - 1]
            log.append(entry(clock, prev.x, prev.y, trace[i].heading))  # hold position while turning
        j = i + 1
        while j < len(trace) and trace[j].turns == 0:
            j += 1
        total = (j - i) * spec.cell_size
        for k in range(j - i):
            st = trace[i + k]
            t = clock + run_time_at((k + 1) * spec.cell_size, total, spec.turn_speed, spec.max_speed, spec.accel)
            log.append(entry(t, st.x, st.y, st.heading))
        clock += run_time_at(total, total, spec.turn_speed, spec.max_speed, spec.accel)
        i = j
    return log


def summarize(maze: MazeData, trace: list, log: list) -> dict:
    optimal = maze.distances[maze.start[1]][maze.start[0]]
    moves = len(trace) - 1
    gap = (moves - optimal) / optimal * 100 if optimal > 0 else 0.0
    return {
        "time_s": log[-1]["t"],
        "path_cells": len(trace),
        "optimal_moves": optimal,
        "gap_pct": round(gap, 1),
        "diagonal_cuts": 0,     # the simulator is orthogonal-only for now
        "blocked_corners": 0,
    }


# --------------------------------------------------------------------------
# pywebview API: everything the UI can call
# --------------------------------------------------------------------------
def _guard(fn):
    """Turn exceptions into an error event + result so the UI never hangs on a failed call."""
    @functools.wraps(fn)
    def wrapper(self, *args, **kwargs):
        try:
            return fn(self, *args, **kwargs)
        except Exception as exc:  # noqa: BLE001 - surfaced to the UI on purpose
            self._push("error", {"message": str(exc)})
            return {"ok": False, "error": str(exc)}
    return wrapper


class Api:
    # pywebview exposes public attributes recursively, so all state is underscore-prefixed.
    def __init__(self, engine: Engine):
        self._engine = engine
        self._window = None
        self._maze = None
        self._mouse = None
        self._history = []
        self._lock = threading.Lock()

    def _bind(self, window):
        self._window = window

    def _push(self, kind: str, data: dict):
        if self._window is not None:
            payload = json.dumps({"kind": kind, "data": data})
            self._window.evaluate_js(f"window.onOrchestratorEvent({payload})")

    def _pick_file(self, file_types):
        dialog = getattr(webview, "FileDialog", None)
        kind = dialog.OPEN if dialog else webview.OPEN_DIALOG  # newer vs older pywebview
        picked = self._window.create_file_dialog(kind, file_types=file_types)
        return picked[0] if picked else None

    @_guard
    def generate_maze(self, size, braid_pct, seed):
        seed = int(seed) if seed not in (None, "") else random.getrandbits(32)
        with self._lock:
            self._maze = self._engine.generate(int(size), seed, int(braid_pct))
        meta = {"seed": seed, "braid_pct": int(braid_pct), "source": "generated", "solvable": True}
        self._push("maze", {"maze": self._maze.to_json(), "meta": meta})
        return {"ok": True}

    @_guard
    def load_maze_file(self):
        path = self._pick_file(("Maze JSON (*.json)", "All files (*.*)"))
        if not path:
            return {"ok": True, "cancelled": True}
        with self._lock:
            self._maze = load_maze_json(path)
        meta = {"seed": None, "braid_pct": None, "source": Path(path).name, "solvable": True}
        self._push("maze", {"maze": self._maze.to_json(), "meta": meta})
        return {"ok": True}

    @_guard
    def load_mouse(self):
        path = self._pick_file(("Mouse library (*.so;*.dll;*.dylib)", "All files (*.*)"))
        if not path:
            return {"ok": True, "cancelled": True}
        with self._lock:
            self._mouse = Mouse(path)
        self._push("mouse", {"name": self._mouse.name})
        return {"ok": True}

    @_guard
    def run_mouse(self, spec):
        if self._maze is None:
            raise RuntimeError("generate or load a maze first")
        if self._mouse is None:
            raise RuntimeError("load a mouse library first")
        spec = MouseSpec.from_dict(spec or {})
        with self._lock:
            trace = simulate(self._maze, self._mouse)
            log = build_run_log(trace, spec)
            summary = summarize(self._maze, trace, log)
            self._history.insert(0, {"id": len(self._history) + 1,
                                     "mouse": self._mouse.name, "time_s": summary["time_s"]})
            maze_json = self._maze.to_json()
            maze_json["path"] = [{"x": s.x, "y": s.y} for s in trace]
            maze_json["run_log"] = log
        self._push("run", {"maze": maze_json, "summary": summary, "history": self._history[:10]})
        return {"ok": True}


def main():
    here = Path(__file__).parent
    # api = Api(Engine(find_lib(here, "mazeviz")))
    api = Api(None)
    window = webview.create_window("Mazeviz", str(here / "ui" / "index.html"),
                                   js_api=api, width=1280, height=800)
    api._bind(window)
    webview.start()


if __name__ == "__main__":
    main()
