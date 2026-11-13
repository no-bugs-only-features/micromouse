"""Maze model, validation, rotation, and the wrapper for the C maze-generator library."""
import ctypes
import random
from collections import deque
from dataclasses import dataclass

from .dllutil import load_copy

N, E, S, W = 8, 4, 2, 1
HEADINGS = "NESW"                                   # heading index 0..3
DIRS = ((0, -1, N), (1, 0, E), (0, 1, S), (-1, 0, W))   # (dx, dy, wall bit), indexed by heading
BIT_TO_HEADING = {N: 0, E: 1, S: 2, W: 3}
# Clockwise quarter-turns that bring each start corner to NW (the mouse frame).
CORNER_TO_ROT = {(False, False): 0, (True, False): 3, (True, True): 2, (False, True): 1}


def rol4(v, k):
    k %= 4; v &= 0xF
    return ((v << k) | (v >> (4 - k))) & 0xF


def rotr4(v, k):
    k %= 4; v &= 0xF
    return ((v >> k) | (v << (4 - k))) & 0xF


def cw(x, y, n, q=1):
    """Cell (x, y) after rotating an n x n grid q quarter-turns clockwise."""
    for _ in range(q % 4):
        x, y = n - 1 - y, x
    return x, y


def rotate_grid(grid, n, q, cell_fn=None):
    out = [[None] * n for _ in range(n)]
    for y in range(n):
        for x in range(n):
            nx, ny = cw(x, y, n, q)
            out[ny][nx] = cell_fn(grid[y][x]) if cell_fn else grid[y][x]
    return out


@dataclass
class Maze:
    size: int
    walls: list          # walls[y][x], N8 E4 S2 W1, low nibble only
    start: tuple
    goal: tuple          # the single cell the mouse is told about
    heading: str = "E"   # the mouse's starting absolute direction (N, E, S or W); east if the file omits it

    def goal_cells(self):
        """Cells that count as 'goal reached': the 2x2 centre block (even size), else the goal cell."""
        n, h = self.size, self.size // 2
        center = [(h, h)] if n % 2 else [(h - 1, h - 1), (h, h - 1), (h - 1, h), (h, h)]
        return center if tuple(self.goal) in center else [tuple(self.goal)]

    def to_json(self):
        return {"size": self.size, "walls": self.walls,
                "start": {"x": self.start[0], "y": self.start[1]},
                "goal": {"x": self.goal[0], "y": self.goal[1]}, "heading": self.heading}

    @classmethod
    def from_json(cls, d):
        g = d["goal"]
        if isinstance(g, list):                    # tolerate the spec's array form
            g = g[0]
        return cls(int(d["size"]),
                   [[int(c) & 0x0F for c in row] for row in d["walls"]],
                   (int(d["start"]["x"]), int(d["start"]["y"])), (int(g["x"]), int(g["y"])),
                   str(d.get("heading", "E")).upper())


def rotate_cw(m, q):
    """Maze rotated q quarter-turns clockwise (cells, wall bits, start and goal)."""
    q %= 4
    if q == 0:
        return m
    heading = HEADINGS[(HEADINGS.index(m.heading) + q) % 4] if m.heading in HEADINGS else m.heading
    return Maze(m.size, rotate_grid(m.walls, m.size, q, lambda c: rotr4(c, q)),
                cw(*m.start, m.size, q), cw(*m.goal, m.size, q), heading)


def bfs_dist(walls, n, sources):
    dist = [[-1] * n for _ in range(n)]
    dq = deque()
    for x, y in sources:
        dist[y][x] = 0
        dq.append((x, y))
    while dq:
        x, y = dq.popleft()
        for dx, dy, bit in DIRS:
            nx, ny = x + dx, y + dy
            if 0 <= nx < n and 0 <= ny < n and not walls[y][x] & bit and dist[ny][nx] < 0:
                dist[ny][nx] = dist[y][x] + 1
                dq.append((nx, ny))
    return dist


def validate(m, strict_goal=False):
    """Return a list of problems (empty = valid). strict_goal also checks the goal-area rules."""
    n, errs = m.size, []
    if n < 3:
        return ["size must be >= 3"]
    if len(m.walls) != n or any(len(r) != n for r in m.walls):
        return ["walls must be size x size"]
    for y in range(n):
        for x in range(n):
            w = m.walls[y][x]
            for dx, dy, bit in DIRS:
                nx, ny = x + dx, y + dy
                if not (0 <= nx < n and 0 <= ny < n):
                    if not w & bit:
                        errs.append(f"outer wall missing at ({x},{y})")
                elif bit in (E, S) and bool(w & bit) != bool(m.walls[ny][nx] & rol4(bit, 2)):
                    errs.append(f"inconsistent wall between ({x},{y}) and ({nx},{ny})")
    sx, sy = m.start
    if not (0 <= sx < n and 0 <= sy < n):
        errs.append("start out of bounds")
    elif not (sx in (0, n - 1) and sy in (0, n - 1)):
        errs.append("start must be a corner cell")
    if m.heading not in tuple(HEADINGS):
        errs.append(f'heading must be one of N, E, S, W (got "{m.heading}")')
    elif 0 <= sx < n and 0 <= sy < n and m.walls[sy][sx] & DIRS[HEADINGS.index(m.heading)][2]:
        errs.append(f"start cell faces a wall (heading {m.heading}); set \"heading\" to an open side")
    if not (0 <= m.goal[0] < n and 0 <= m.goal[1] < n):
        errs.append("goal out of bounds")
    if strict_goal and not errs:
        area = set(m.goal_cells()); entrances = 0
        for x, y in area:
            for dx, dy, bit in DIRS:
                inside = (x + dx, y + dy) in area
                if inside and m.walls[y][x] & bit:
                    errs.append("goal area has an inner wall"); break
                if not inside and 0 <= x + dx < n and 0 <= y + dy < n and not m.walls[y][x] & bit:
                    entrances += 1
        if entrances != 1:
            errs.append(f"goal area has {entrances} entrances (expected 1)")
    if not errs and bfs_dist(m.walls, n, m.goal_cells())[sy][sx] < 0:
        errs.append("goal is not reachable from start")
    return errs[:6] + ([f"(+{len(errs) - 6} more)"] if len(errs) > 6 else [])


class GeneratorError(Exception):
    pass


class MazeGenerator:
    """Wraps the C library: Maze generate(int size, int seed); void maze_free(Maze, int size)."""

    def __init__(self, dll_path):
        self.path = dll_path

    def _lib(self):
        lib = load_copy(self.path)
        row_pp = ctypes.POINTER(ctypes.POINTER(ctypes.c_ubyte))
        lib.generate.argtypes = [ctypes.c_int, ctypes.c_int]
        lib.generate.restype = row_pp
        lib.maze_free.argtypes = [row_pp, ctypes.c_int]
        lib.maze_free.restype = None
        return lib

    def generate(self, size=None, seed=None):
        size = 16 if size is None else int(size)
        if not 3 <= size <= 64:
            raise GeneratorError("size must be between 3 and 64")
        seed = random.randrange(2 ** 31) if seed is None else int(seed)
        if seed < 0:
            raise GeneratorError("seed must be >= 0 (-1 would seed from the clock and lose the seed)")
        lib = self._lib()
        raw = lib.generate(size, seed)
        if not raw:
            raise GeneratorError("generate() returned NULL")
        try:
            walls = [[raw[y][x] & 0x0F for x in range(size)] for y in range(size)]   # copy out, drop upper nibble
        finally:
            lib.maze_free(raw, size)
        heading = "E" if not walls[0][0] & E else "S"          # face an open side of the start cell
        m = Maze(size, walls, (0, 0), (size // 2, size // 2), heading)
        errs = validate(m, strict_goal=True)
        if errs:
            raise GeneratorError("invalid maze (seed %d): %s" % (seed, "; ".join(errs)))
        return m, seed
