"""Micromouse simulator: pywebview window + Python API for sim.html."""
import argparse
import json
import os
import threading
import traceback

import webview

from src.engine import Sim
from src.maze import GeneratorError, Maze, MazeGenerator, validate
from src.mouse_dll import Mouse

HERE = os.path.dirname(os.path.abspath(__file__))


# ---- verbose console output (ASCII only, so any Windows console code page is fine) ----
def _sensed(w):
    if w is None:
        return ""
    return ("L" if w & 1 else "") + ("F" if w & 8 else "") + ("R" if w & 4 else "") or "-"


def _belief(f):
    b = f["believed"]
    return f"({b['pos']['x']},{b['pos']['y']}) {b['heading']}" if b else "unresolved"


def _off(f):
    b = f["believed"]
    return bool(b) and (b["pos"] != f["pos"] or b["heading"] != f["heading"])


def _notes(prev, f):
    if prev is None:
        return "start"
    n = []
    if f["believed"] and not prev["believed"]:
        n.append("heading resolved: " + f["believed"]["heading"])
    if f["goal_reached"] and not prev["goal_reached"]:
        n.append("goal reached")
    if _off(f) and not _off(prev):
        n.append("belief diverged")
    if _off(prev) and not _off(f):
        n.append("belief recovered")
    if f["status"] != "running":
        n.append(f["status"] + (": " + f["message"] if f["message"] else ""))
    return " | ".join(n)


def _row(prev, f):
    pos = f"({f['pos']['x']},{f['pos']['y']})"
    return (f"[run] {f['step']:6d}  {_sensed(f['sensed']):<4} {(f['action'] or '-'):<9} {pos:<9} "
            f"{f['heading']:<3} {_belief(f):<14} {_notes(prev, f)}").rstrip()


class Api:
    # Names starting with "_" are private: pywebview does not expose them to JavaScript.
    def __init__(self, mouse_dll, gen_dll, verbose=False):
        self._mouse = Mouse(mouse_dll)
        self._gen = MazeGenerator(gen_dll)
        self._verbose = verbose
        self._sim = None
        self._prev = None                       # previous frame, for run-log notes
        self._gen_json, self._gen_seed = None, None   # last generated maze and its seed
        self._lock = threading.Lock()

    def _log(self, msg):
        if self._verbose:
            print(msg, flush=True)

    def _guard(self, fn):
        try:
            with self._lock:
                return fn()
        except Exception as e:                       # report to the page instead of dying silently
            traceback.print_exc()
            return {"error": f"{type(e).__name__}: {e}"}

    def generate_maze(self, size, seed):
        def go():
            try:
                maze, used = self._gen.generate(size, seed)
            except GeneratorError as e:
                return {"error": str(e)}
            self._gen_json, self._gen_seed = maze.to_json(), used
            self._log(f"[maze] generated size={maze.size} seed={used}")
            return {"maze": self._gen_json, "seed": used}
        return self._guard(go)

    def start(self, maze, opts):
        def go():
            base = Maze.from_json(maze)
            errs = validate(base)
            if errs:
                return {"error": "; ".join(errs)}
            rotate = bool((opts or {}).get("random_corner"))
            sim = Sim(self._mouse, (opts or {}).get("max_steps"))
            rot_seed = (opts or {}).get("rotation_seed")   # Reset passes the previous one back for an identical start
            frame = sim.start(base, rotate, None if rot_seed is None else int(rot_seed))
            self._sim, self._prev = sim, None
            if self._verbose:
                maze_seed = f"maze seed={self._gen_seed}" if maze == self._gen_json else "maze seed=n/a (loaded maze)"
                rot = f"rotation seed={sim.seed}" if rotate else "no rotation"
                self._log(f"[start] {maze_seed} | {rot} | frame={sim.mouse_frame_deg} deg (true->mouse, clockwise) | "
                          f"facing={sim.start_heading_name} | start=({sim.maze.start[0]},{sim.maze.start[1]}) "
                          f"goal=({sim.maze.goal[0]},{sim.maze.goal[1]})")
                self._log("[run]   step  seen action    pos       hdg believed       notes")
                self._log(_row(None, frame))
            self._prev = frame
            return {"maze": sim.maze.to_json(), "frame": frame, "mouse_frame": sim.mouse_frame_deg,
                    "start_heading": sim.start_heading_name, "seed": sim.seed}
        return self._guard(go)

    def step(self):
        def go():
            if self._sim is None:
                return {"error": "no session: call start first"}
            was_running = self._prev is None or self._prev["status"] == "running"
            frame = self._sim.step()
            if self._verbose and was_running:             # don't repeat the final row if polled again
                self._log(_row(self._prev, frame))
                if frame["status"] != "running":
                    self._log(f"[end] status={frame['status']} steps={frame['step']} "
                              f"goal_reached={frame['goal_reached']} goal_step={frame['goal_step']}")
            self._prev = frame
            return frame
        return self._guard(go)

    def export_maze(self):
        """Save the maze exactly as the sim is running it (true frame: size, walls, start, goal, heading)."""
        def go():
            if self._sim is None:
                return {"error": "nothing to export"}
            os.makedirs(os.path.join(HERE, "exports"), exist_ok=True)
            path = os.path.join(HERE, "exports", "maze.json")
            with open(path, "w", encoding="utf-8") as f:
                json.dump(self._sim.maze.to_json(), f)
            self._log(f"[export] {path}")
            return {"path": path}
        return self._guard(go)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--mouse", default=os.path.join(HERE, "..", "build", "sim", "libmouse.dll"))
    ap.add_argument("--gen", default=os.path.join(HERE, "..", "build", "sim", "libmazegen.dll"))
    ap.add_argument("-v", "--verbose", action="store_true",
                    help="print the maze seed, frame and facing, and the run log to the console")
    ap.add_argument("--debug", action="store_true", help="open the webview dev tools")
    args = ap.parse_args()
    webview.create_window("Micromouse Sim", os.path.join(HERE, "ui", "sim.html"),
                          js_api=Api(args.mouse, args.gen, args.verbose), width=1280, height=820)
    webview.start(debug=args.debug)


if __name__ == "__main__":
    main()
