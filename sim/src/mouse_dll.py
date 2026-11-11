"""ctypes wrapper around the firmware shared library.

The firmware is stateless: this class owns the mouse's map, position, direction and pre-resolution
wall buffer as plain memory and hands them to mouse_step() on every call. There is no init function.
A fresh mouse is all-zero memory: CellID 0 = (0,0), direction 0 = undefined, empty buffer.
"""
import ctypes

# Order must match `action_t` in the firmware's header.
ACTIONS = ("FORWARD", "TURN_L", "TURN_R", "TURN_180", "DONE")
_DIR_INDEX = {8: 0, 4: 1, 2: 2, 1: 3}        # Direction enum (N8 E4 S2 W1) -> heading index N,E,S,W
_GUARD = 16                                   # slack after the pending buffer so an overrun can't corrupt memory
CELL_NONE = 0xFFFF                            # marks unused slots in the path array
_PAD = 8                                      # guard bytes around the map: one cell outside it is harmless and detected

_ROWS = ctypes.POINTER(ctypes.POINTER(ctypes.c_ubyte))     # Maze = Cell ** (m[y][x])
_ARGS = [ctypes.c_uint8, _ROWS, ctypes.c_int,
         ctypes.POINTER(ctypes.c_uint16),     # CellID *pos
         ctypes.POINTER(ctypes.c_uint),       # Direction *dir (enum; unsigned int under GCC)
         ctypes.POINTER(ctypes.c_uint16),     # CellID *path (size*size entries, pre-filled with CELL_NONE every call)
         ctypes.POINTER(ctypes.c_ubyte),      # Cell *pending
         ctypes.POINTER(ctypes.c_uint16)]     # uint16_t *pending_len


class Mouse:
    def __init__(self, dll_path):
        self.dll_path = dll_path                 # the firmware library on disk
        self.size = 0
        self.lib = None

    def _bind(self):
        from .dllutil import load_copy
        lib = load_copy(self.dll_path)           # picks up a rebuilt DLL automatically
        lib.mouse_step.argtypes = _ARGS
        lib.mouse_step.restype = ctypes.c_int
        self._has_dist = hasattr(lib, "mouse_dbg_dist")
        if self._has_dist:
            lib.mouse_dbg_dist.argtypes = [_ROWS, ctypes.c_int, ctypes.POINTER(ctypes.c_int16)]
            lib.mouse_dbg_dist.restype = None
        return lib

    def reset(self, size):
        """Fresh mouse: empty map, CellID 0, direction undefined, empty pending buffer (capacity = size)."""
        self.lib = self._bind()
        self.size = size
        # Map rows with guard space on every side: two extra guard rows (above/below) and _PAD spare bytes
        # left/right of each row, so an unchecked neighbour write at the boundary (maze[-1][x], maze[size][x],
        # maze[y][-1], maze[y][size]) lands in padding instead of corrupting memory. fault() reports any hit.
        w = size + 2 * _PAD
        self._raw = [(ctypes.c_ubyte * w)() for _ in range(size + 2)]        # [0] guard above, [1..size] rows, [size+1] guard below
        self._ptrs = (ctypes.POINTER(ctypes.c_ubyte) * (size + 2))(
            *[ctypes.cast(ctypes.addressof(r) + _PAD, ctypes.POINTER(ctypes.c_ubyte)) for r in self._raw])
        self.rows = ctypes.cast(ctypes.addressof(self._ptrs) + ctypes.sizeof(ctypes.c_void_p), _ROWS)   # points at row 0
        self.pos, self.dir, self.pending_len = ctypes.c_uint16(0), ctypes.c_uint(0), ctypes.c_uint16(0)
        self.pending = (ctypes.c_ubyte * (size + _GUARD))()
        self._path_buf = (ctypes.c_uint16 * (size * size + _GUARD))()          # + slack so an overrun is detected, not fatal
        self._path_cells, self._path_err = [], None

    def step(self, walls):
        ctypes.memset(self._path_buf, 0xFF, ctypes.sizeof(self._path_buf))        # empty path: every slot is CELL_NONE
        code = self.lib.mouse_step(walls, self.rows, self.size, ctypes.byref(self.pos), ctypes.byref(self.dir),
                                   self._path_buf, ctypes.cast(self.pending, ctypes.POINTER(ctypes.c_ubyte)),
                                   ctypes.byref(self.pending_len))
        self._path_cells, self._path_err = self._scan_path()
        return code

    def _scan_path(self):
        """The CellIDs the firmware wrote (up to the first CELL_NONE), plus an error string if they are garbage."""
        n2 = self.size * self.size
        raw = list(self._path_buf)                                          # includes the guard slots after n2
        try:
            end = raw.index(CELL_NONE)
        except ValueError:
            end = len(raw)
        cells = raw[:min(end, n2)]
        for i, v in enumerate(cells):
            if v >= n2:
                return cells[:i], f"path[{i}] = {v} is not a valid CellID (size*size = {n2})"
        if any(v != CELL_NONE for v in raw[n2:]):
            return cells, f"path buffer overflow: firmware wrote past {n2} entries"
        return cells, None

    def plan(self):
        """The mouse's intended path as CellIDs in the mouse frame ([] when the firmware wrote none)."""
        return list(self._path_cells)

    def _outside(self):
        """Describe the first write outside the map (guard rows / side padding), or None."""
        n = self.size
        for i in range(n + 2 * _PAD):
            if self._raw[0][i]:
                return f"north of ({i - _PAD},0)"
            if self._raw[n + 1][i]:
                return f"south of ({i - _PAD},{n - 1})"
        for y in range(n):
            r = self._raw[y + 1]
            for i in range(_PAD):
                if r[i]:
                    return f"west of (0,{y})"
                if r[_PAD + n + i]:
                    return f"east of ({n - 1},{y})"
        return None

    def fault(self):
        if self.pending_len.value > self.size:
            return f"pending buffer overflow: length {self.pending_len.value} > capacity {self.size}"
        if self._path_err:
            return self._path_err
        where = self._outside()
        if where:
            return f"firmware wrote outside the maze, {where} (unchecked neighbour write at the boundary?)"
        return None

    # ---- what the mouse believes, read from the memory it was handed (mouse frame) ----
    def pose(self):
        n, d = self.size, self.dir.value
        return {"x": self.pos.value % n, "y": self.pos.value // n,
                "heading": _DIR_INDEX.get(d, -1), "resolved": d in _DIR_INDEX}

    def known(self):
        n = self.size
        return [list(self._raw[y + 1][_PAD:_PAD + n]) for y in range(n)]

    def dist(self):                          # optional getter; None if the firmware doesn't export it
        if not self._has_dist:
            return None
        n = self.size
        buf = (ctypes.c_int16 * (n * n))()
        self.lib.mouse_dbg_dist(self.rows, n, buf)
        return [list(buf[y * n:(y + 1) * n]) for y in range(n)]
