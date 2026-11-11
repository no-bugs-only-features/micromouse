"""Load a DLL from a temp copy so rebuilding never hits Windows' file lock.

The copy is cached by source path + mtime: a rebuilt DLL is picked up automatically
(old copy is freed), an unchanged one is reused.
"""
import atexit
import ctypes
import os
import shutil
import tempfile

_cache = {}      # source path -> (mtime, lib, copy_path)
_dirs = set()    # directories already added to the DLL search path
_tmp = None


def _unload(lib, copy):
    if os.name == "nt":
        from ctypes import wintypes
        k32 = ctypes.WinDLL("kernel32")
        k32.FreeLibrary.argtypes = [wintypes.HMODULE]
        k32.FreeLibrary(lib._handle)
    try:
        os.remove(copy)
    except OSError:
        pass


def load_copy(path):
    global _tmp
    path = os.path.abspath(path)
    if not os.path.isfile(path):
        raise FileNotFoundError(f"DLL not found: {path}")
    mtime = os.path.getmtime(path)
    ent = _cache.get(path)
    if ent and ent[0] == mtime:
        return ent[1]
    if ent:                                  # source was rebuilt
        _unload(ent[1], ent[2])
    if _tmp is None:
        _tmp = tempfile.mkdtemp(prefix="mmsim_")
    base = os.path.splitext(os.path.basename(path))[0]
    copy = os.path.join(_tmp, f"{base}_{int(mtime * 1000)}.dll")
    shutil.copy2(path, copy)
    src_dir = os.path.dirname(path)
    # MinGW runtime DLLs (libgcc, libwinpthread) next to the original must stay findable.
    # Linking with -static -static-libgcc avoids the issue entirely.
    if hasattr(os, "add_dll_directory") and src_dir not in _dirs:
        os.add_dll_directory(src_dir)
        _dirs.add(src_dir)
    lib = ctypes.CDLL(copy)
    _cache[path] = (mtime, lib, copy)
    return lib


@atexit.register
def _cleanup():
    for _, lib, copy in list(_cache.values()):
        _unload(lib, copy)
    _cache.clear()
    if _tmp:
        shutil.rmtree(_tmp, ignore_errors=True)
