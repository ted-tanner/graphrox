from enum import Enum
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


class _GphrxErrorCode(Enum):
    GPHRX_NO_ERROR = 0
    GPHRX_ERROR_NOT_FOUND = 1
    GPHRX_ERROR_INVALID_FORMAT = 2


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

_gphrx_lib.duplicate_gphrx.argtypes = [ctypes.POINTER(_GphrxGraph_c)]
_gphrx_lib.duplicate_gphrx.restype = _GphrxGraph_c

_gphrx_lib.free_gphrx.argtypes = [ctypes.POINTER(_GphrxGraph_c)]
_gphrx_lib.free_gphrx.restype = None

_gphrx_lib.gphrx_shrink.argtypes = [ctypes.POINTER(_GphrxGraph_c)]
_gphrx_lib.gphrx_shrink.restype = None

_gphrx_lib.gphrx_does_edge_exist.argtypes = (ctypes.POINTER(_GphrxGraph_c), ctypes.c_uint64, ctypes.c_uint64)
_gphrx_lib.gphrx_does_edge_exist.restype = ctypes.c_bool

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


class GphrxGraph:
    def __init__(self, is_undirected=True, byte_array=None):
        if byte_array is not None:
            pass
        else: 
            self.is_undirected = is_undirected
            self.highest_vertex_id = 0
            self._graph = _gphrx_lib.new_undirected_gphrx() if is_undirected else _gphrx_lib.new_directed_gphrx()

    def __del__(self):
        _gphrx_lib.free_gphrx(self._graph)

    @staticmethod
    def from_bytes(byte_array):
        error_code = ctypes.c_uint8()
        arr = (ctypes.c_ubyte * (len(byte_array))).from_buffer(bytearray(byte_array))
        c_graph = _gphrx_lib.gphrx_from_byte_array(arr, ctypes.byref(error_code))

        if error_code.value != _GphrxErrorCode.GPHRX_NO_ERROR.value:
            raise ValueError("GrphxGraph could not be constructed from the provided bytes")

        graph = GphrxUndirectedGraph() if c_graph.is_undirected else GphrxDirectedGraph()
        graph.highest_vertex_id = c_graph.highest_vertex_id
        graph._graph = c_graph
        return graph

    def duplicate(self):
        c_graph = _gphrx_lib.duplicate_gphrx(self._graph)
        
        graph = GphrxUndirectedGraph() if c_graph.is_undirected else GphrxDirectedGraph()
        graph.highest_vertex_id = c_graph.highest_vertex_id
        graph._graph = c_graph

        return graph
        
    def shrink(self):
        _gphrx_lib.gphrx_shrink(self._graph)

    def does_edge_exist(self, from_vertex_id, to_vertex_id):
        return _gphrx_lib.gphrx_does_edge_exist(self._graph, from_vertex_id, to_vertex_id)

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
        error_code = _gphrx_lib.gphrx_remove_edge(self._graph, from_vertex_id, to_vertex_id)

        if error_code == _GphrxErrorCode.GPHRX_ERROR_NOT_FOUND.value:
            raise ValueError("Edge from vertex " + str(from_vertex_id) +
                             " to vertex " + str(to_vertex_id) + " does not exist")
        
        if self._graph.highest_vertex_id != self.highest_vertex_id:
            self.highest_vertex_id = self._graph.highest_vertex_id

    def save_to_file(self, file_name):
        with open(file_name, 'wb') as f:
            f.write(bytes(self))

    @staticmethod
    def load_from_file(file_name):
        with open(file_name, 'rb') as f:
            return GphrxGraph.from_bytes(f.read())

    def __bytes__(self):
        HEADER_SIZE_IN_BYTES = 26
        HEADER_POS_OF_MATRIX_DIMENSION = 16
        
        byte_array_ptr = _gphrx_lib.gphrx_to_byte_array(self._graph)
        
        dimension_bytes = byte_array_ptr[HEADER_POS_OF_MATRIX_DIMENSION:
                                         HEADER_POS_OF_MATRIX_DIMENSION + ctypes.sizeof(ctypes.c_uint64)]
        dimension = int.from_bytes(dimension_bytes, byteorder='big', signed=False)

        total_array_size = HEADER_SIZE_IN_BYTES + dimension * 2 * ctypes.sizeof(ctypes.c_uint64)
        bytes_obj = bytes(byte_array_ptr[:total_array_size])

        _gphrx_lib.free_gphrx_byte_array(byte_array_ptr)

        return bytes_obj


class GphrxUndirectedGraph(GphrxGraph):
    def __init__(self):
        super().__init__(True)
        

class GphrxDirectedGraph(GphrxGraph):
    def __init__(self):
        super().__init__(False)


if __name__ == '__main__':
    test = GphrxUndirectedGraph()
    print(test)
    
    print(test.highest_vertex_id)
    test.add_edge(10, 100)
    print(test.highest_vertex_id)
    test.add_vertex(122, [1, 10, 100, 1001])
    test = test.duplicate()
    test.add_edge(10, 100)
    print(test.does_edge_exist(10, 100))
    print(test.highest_vertex_id)

    # test.save_to_file('./test.gphrx')
    
    test.remove_edge(122, 10)
    test.remove_vertex(122)
    test.shrink()

    test2 = GphrxGraph.from_bytes(bytes(test))
    print(test2.highest_vertex_id)
