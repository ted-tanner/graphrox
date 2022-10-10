from pathlib import Path

import ctypes
import os
import platform


_libs_path = os.path.join(Path(__file__).resolve().parent, 'lib')

_os_name = platform.system()

if _os_name == 'Linux':
    _dll_name = 'graphrox.so'
elif _os_name == 'Windows':
    _dll_name = 'graphrox.dll'
elif _os_name == 'Darwin':
    if 'x86_64' == platform.uname().machine:
        _dll_name = 'graphrox-x86.dylib'
    else:
        _dll_name = 'graphrox-arm64.dylib'
else:
    raise ImportError('GraphRox module not supported on this system')


_gphrx_lib_path = os.path.join(_libs_path, _dll_name)
_gphrx_lib = ctypes.cdll.LoadLibrary(_gphrx_lib_path)


class _DynamicArrayU64(ctypes.Structure):
    _fields_ = [
        ("capacity", ctypes.c_size_t),
        ("size", ctypes.c_size_t),
        ("arr", ctypes.POINTER(ctypes.c_uint64))]
    
class _CsrAdjMatrix(ctypes.Structure):
    _fields_ = [
        ("matrix_col_idx_list", _DynamicArrayU64),
        ("matrix_row_idx_list", _DynamicArrayU64)]

class _GphrxGraph(ctypes.Structure):
    _fields_ = [
        ("is_undirected", ctypes.c_bool),
        ("highest_vertex_id", ctypes.c_uint64),
        ("adjacency_matrix", _CsrAdjMatrix)]

_gphrx_lib_path = os.path.join(_libs_path, _dll_name)
_gphrx_lib = ctypes.cdll.LoadLibrary(_gphrx_lib_path)

_gphrx_lib.new_undirected_gphrx.argtypes = None
_gphrx_lib.new_undirected_gphrx.restype = _GphrxGraph

_gphrx_lib.new_directed_gphrx.argtypes = None
_gphrx_lib.new_directed_gphrx.restype = _GphrxGraph

_gphrx_lib.free_gphrx.argtypes = [ctypes.POINTER(_GphrxGraph)]
_gphrx_lib.free_gphrx.restype = None

_gphrx_lib.gphrx_shrink.argtypes = [ctypes.POINTER(_GphrxGraph)]
_gphrx_lib.gphrx_shrink.restype = None

_gphrx_lib.gphrx_add_vertex.argtypes = (ctypes.POINTER(_GphrxGraph),
                                         ctypes.c_uint64,
                                         ctypes.POINTER(ctypes.c_uint64),
                                         ctypes.c_uint64)
_gphrx_lib.gphrx_add_vertex.restype = None

_gphrx_lib.gphrx_remove_vertex.argtypes = (ctypes.POINTER(_GphrxGraph), ctypes.c_uint64)
_gphrx_lib.gphrx_remove_vertex.restype = None

_gphrx_lib.gphrx_add_edge.argtypes = (ctypes.POINTER(_GphrxGraph), ctypes.c_uint64, ctypes.c_uint64)
_gphrx_lib.gphrx_add_edge.restype = None

_gphrx_lib.gphrx_remove_edge.argtypes = (ctypes.POINTER(_GphrxGraph), ctypes.c_uint64, ctypes.c_uint64)
_gphrx_lib.gphrx_remove_edge.restype = ctypes.c_uint8

_gphrx_lib.gphrx_to_byte_array.argtypes = (ctypes.POINTER(_GphrxGraph), ctypes.c_uint64, ctypes.c_uint64)
_gphrx_lib.gphrx_to_byte_array.restype = ctypes.POINTER(ctypes.c_ubyte)

_gphrx_lib.gphrx_to_byte_array.argtypes = (ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_uint8))
_gphrx_lib.gphrx_to_byte_array.restype = _GphrxGraph





test = _gphrx_lib.new_undirected_gphrx()

print(test)

_gphrx_lib.free_gphrx(test)
