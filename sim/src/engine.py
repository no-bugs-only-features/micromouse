"""Simulator core: true pose, detect(), step(), frame conversion for the visualizer."""
import random

from . import maze as M
from .mouse_dll import ACTIONS


class Sim:
    def __init__(self, mouse, max_steps=None):
        self.mouse = mouse
        self.max_steps_opt = max_steps

    # ---- session setup ----
    def start(self, base, random_corner=False, seed=None):
        self.seed = seed if seed is not None else random.randrange(2 ** 31)
        k = random.Random(self.seed).randrange(4) if random_corner else 0
        self.maze = M.rotate_cw(base, k)                  # the true frame the page draws
        n = self.maze.size
        sx, sy = self.maze.start
        self.heading = self.start_heading = M.HEADINGS.index(self.maze.heading)   # from the maze file (default E)
        self.mouse_rot = M.CORNER_TO_ROT[(sx == n - 1, sy == n - 1)]   # CW turns: true frame -> mouse frame
        self.q = (4 - self.mouse_rot) % 4                              # CW turns: mouse frame -> true frame
        self.goal_set = set(self.maze.goal_cells())
        self.pos = (sx, sy)
        self.n_steps, self.status, self.message = 0, "running", ""
        self.goal_step, self.last_action, self.last_sense = None, None, None
        self.max_steps = self.max_steps_opt or 50 * n * n
        self.log = [(0, sx, sy, self.heading)]
        self.mouse.reset(n)                           # empty map, CellID 0, direction undefined
        return self._frame()

    @property
    def mouse_frame_deg(self):
        return self.mouse_rot * 90

    @property
    def start_heading_name(self):
        return M.HEADINGS[self.start_heading]

    # ---- the reading handed to the firmware each step: walls of the current cell relative to the
    #      mouse's true heading (forward 8, right 4, left 1, back always 0) ----
    def _sense(self):
        x, y = self.pos
        return M.rol4(self.maze.walls[y][x], self.heading) & 0b1101

    # ---- one firmware step ----
    def step(self):
        if self.status != "running":
            return self._frame()
        self.n_steps += 1
        self.last_sense = self._sense()
        code = self.mouse.step(self.last_sense)
        x, y = self.pos
        fault = self.mouse.fault()
        if fault:
            self._end("invalid", fault)
        elif not 0 <= code < len(ACTIONS):
            self.last_action = f"?{code}"
            self._end("invalid", f"firmware returned unknown action {code}")
        else:
            self.last_action = ACTIONS[code]
            if self.last_action == "FORWARD":
                dx, dy, bit = M.DIRS[self.heading]
                nx, ny = x + dx, y + dy
                n = self.maze.size
                if self.maze.walls[y][x] & bit or not (0 <= nx < n and 0 <= ny < n):
                    self._end("collision", f"hit a wall at ({x},{y}) facing {M.HEADINGS[self.heading]}")
                else:
                    self.pos = (nx, ny)
                    if self.goal_step is None and self.pos in self.goal_set:
                        self.goal_step = self.n_steps
            elif self.last_action == "TURN_L":
                self.heading = (self.heading + 3) % 4
            elif self.last_action == "TURN_R":
                self.heading = (self.heading + 1) % 4
            elif self.last_action == "TURN_180":
                self.heading = (self.heading + 2) % 4
            else:
                self._end("done", "firmware called done")
        if self.status == "running" and self.n_steps >= self.max_steps:
            self._end("timeout", f"no done() after {self.max_steps} steps")
        self.log.append((self.n_steps, *self.pos, self.heading))
        return self._frame()

    def _end(self, status, message):
        self.status, self.message = status, message

    # ---- frames (everything in the true frame) ----
    def _frame(self):
        return {"step": self.n_steps, "action": self.last_action, "sensed": self.last_sense,
                "pos": {"x": self.pos[0], "y": self.pos[1]}, "heading": M.HEADINGS[self.heading],
                "status": self.status, "message": self.message, "plan": self._plan(),
                "goal_reached": self.goal_step is not None, "goal_step": self.goal_step,
                "believed": self._believed(), "known": self._known(), "dist": self._dist()}

    def _plan(self):
        """The mouse's intended path as true-frame cells, or None when the firmware wrote none."""
        ids = self.mouse.plan()
        if not ids:
            return None
        n = self.maze.size
        cells = [M.cw(i % n, i // n, n, self.q) for i in ids]
        return [{"x": x, "y": y} for x, y in cells]

    def _believed(self):
        p = self.mouse.pose()
        n = self.maze.size
        if not p or not p["resolved"] or not (0 <= p["x"] < n and 0 <= p["y"] < n) or not 0 <= p["heading"] < 4:
            return None
        x, y = M.cw(p["x"], p["y"], n, self.q)
        return {"pos": {"x": x, "y": y}, "heading": M.HEADINGS[(p["heading"] + self.q) % 4]}

    def _known(self):
        raw = self.mouse.known()
        if raw is None:
            return None
        q = self.q
        return M.rotate_grid(raw, self.maze.size, q,
                             lambda c: M.rotr4(c & 0xF, q) | (M.rotr4((c >> 4) & 0xF, q) << 4))

    def _dist(self):
        raw = self.mouse.dist()
        return None if raw is None else M.rotate_grid(raw, self.maze.size, self.q)
