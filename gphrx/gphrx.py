from pathlib import Path

import ctypes
import os
import platform
import sys


class _DynamicArrayU64_c(ctypes.Structure):
    _fields_ = [
        ("capacity", ctypes.c_size_t),
        ("size", ctypes.c_size_t),
        ("arr", ctypes.POINTER(ctypes.c_uint64))]

    
class _CsrAdjMatrix_c(ctypes.Structure):
    _fields_ = [
        ("matrix_col_idx_list", _DynamicArrayU64_c),
        ("matrix_row_idx_list", _DynamicArrayU64_c)]

    
class _GphrxGraph_c(ctypes.Structure):
    _fields_ = [
        ("is_undirected", ctypes.c_bool),
        ("highest_vertex_id", ctypes.c_uint64),
        ("adjacency_matrix", _CsrAdjMatrix_c)]


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
    
_gphrx_lib_path = os.path.join(_libs_path, _dll_name)
_gphrx_lib = ctypes.cdll.LoadLibrary(_gphrx_lib_path)

_gphrx_lib.new_undirected_gphrx.argtypes = None
_gphrx_lib.new_undirected_gphrx.restype = _GphrxGraph_c

_gphrx_lib.new_directed_gphrx.argtypes = None
_gphrx_lib.new_directed_gphrx.restype = _GphrxGraph_c

_gphrx_lib.free_gphrx.argtypes = [ctypes.POINTER(_GphrxGraph_c)]
_gphrx_lib.free_gphrx.restype = None

_gphrx_lib.gphrx_shrink.argtypes = [ctypes.POINTER(_GphrxGraph_c)]
_gphrx_lib.gphrx_shrink.restype = None

_gphrx_lib.gphrx_add_vertex.argtypes = (ctypes.POINTER(_GphrxGraph_c),
                                        ctypes.c_uint64,
                                        ctypes.POINTER(ctypes.c_uint64),
                                        ctypes.c_uint64)
_gphrx_lib.gphrx_add_vertex.restype = None

_gphrx_lib.gphrx_remove_vertex.argtypes = (ctypes.POINTER(_GphrxGraph_c), ctypes.c_uint64)
_gphrx_lib.gphrx_remove_vertex.restype = None

_gphrx_lib.gphrx_add_edge.argtypes = (ctypes.POINTER(_GphrxGraph_c), ctypes.c_uint64, ctypes.c_uint64)
_gphrx_lib.gphrx_add_edge.restype = None

_gphrx_lib.gphrx_remove_edge.argtypes = (ctypes.POINTER(_GphrxGraph_c), ctypes.c_uint64, ctypes.c_uint64)
_gphrx_lib.gphrx_remove_edge.restype = ctypes.c_uint8

_gphrx_lib.gphrx_to_byte_array.argtypes = [ctypes.POINTER(_GphrxGraph_c)]
_gphrx_lib.gphrx_to_byte_array.restype = ctypes.POINTER(ctypes.c_ubyte)

_gphrx_lib.gphrx_from_byte_array.argtypes = [ctypes.POINTER(ctypes.c_ubyte)]
_gphrx_lib.gphrx_from_byte_array.restype = _GphrxGraph_c

_gphrx_lib.free_gphrx_byte_array.argtypes = [ctypes.c_void_p]
_gphrx_lib.free_gphrx_byte_array.restype = None


class _GphrxGraph:
    def __init__(self, is_undirected):
        self.is_undirected = is_undirected
        self.highest_vertex_id = 0
        self._graph = _gphrx_lib.new_undirected_gphrx() if is_undirected else _gphrx_lib.new_directed_gphrx()

    def __del__(self):
        _gphrx_lib.free_gphrx(self._graph)

    def shrink(self):
        _gphrx_lib.gphrx_shrink(self._graph)

    def add_vertex(self, vertex_id, vertex_edges):
        edges_arr = ctypes.c_uint64 * len(vertex_edges)
        _gphrx_lib.gphrx_add_vertex(self._graph, vertex_id, edges_arr(*vertex_edges), len(vertex_edges))

        if self._graph.highest_vertex_id != self.highest_vertex_id:
            self.highest_vertex_id = self._graph.highest_vertex_id

    def remove_vertex(self, vertex_id):
        _gphrx_lib.gphrx_remove_vertex(self._graph, vertex_id)

        if self._graph.highest_vertex_id != self.highest_vertex_id:
            self.highest_vertex_id = self._graph.highest_vertex_id

    def add_edge(self, from_vertex_id, to_vertex_id):
        _gphrx_lib.gphrx_add_edge(self._graph, from_vertex_id, to_vertex_id)

        if self._graph.highest_vertex_id != self.highest_vertex_id:
            self.highest_vertex_id = self._graph.highest_vertex_id

    def remove_edge(self, from_vertex_id, to_vertex_id):
        _gphrx_lib.gphrx_remove_edge(self._graph, from_vertex_id, to_vertex_id)

        if self._graph.highest_vertex_id != self.highest_vertex_id:
            self.highest_vertex_id = self._graph.highest_vertex_id

    def __bytes__(self):
        HEADER_SIZE_IN_BYTES = 26
        HEADER_POS_OF_MATRIX_DIMENSION = 16
        UINT64_SIZE_IN_BYTES = 8
        
        byte_array_ptr = _gphrx_lib.gphrx_to_byte_array(self._graph)
        
        header = (ctypes.c_uint8 * HEADER_SIZE_IN_BYTES).from_buffer(
            ctypes.cast(byte_array_ptr, ctypes.POINTER(ctypes.c_ubyte * HEADER_SIZE_IN_BYTES)))
        
        dimension_bytes = header[HEADER_POS_OF_MATRIX_DIMENSION:UINT64_SIZE_IN_BYTES]
        dimension = int.from_bytes(dimension_bytes, byteorder='big', signed=False)
        
        total_array_size = HEADER_SIZE_IN_BYTES + dimension * 2 * UINT64_SIZE_IN_BYTES
        byte_array = (ctypes.c_uint8 * total_array_size).from_buffer(byte_array_ptr)

        bytes_obj = bytes(byte_array)

        _gphrx_lib.free_gphrx_byte_array(byte_array_ptr)

        return bytes_obj


class GphrxUndirectedGraph(_GphrxGraph):
    def __init__(self):
        super().__init__(True)


class GphrxDirectedGraph(_GphrxGraph):
    def __init__(self):
        super().__init__(False)


if __name__ == '__main__':
    test = GphrxUndirectedGraph()
    
    print(test.highest_vertex_id)
    test.add_edge(10, 100)
    print(test.highest_vertex_id)
    test.add_vertex(122, [1, 10, 100, 1001])
    test.add_edge(10, 100)
    print(test.highest_vertex_id)

