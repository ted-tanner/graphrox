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

    
class _DynamicArrayDouble_c(ctypes.Structure):
    _fields_ = [
        ("capacity", ctypes.c_size_t),
        ("size", ctypes.c_size_t),
        ("arr", ctypes.POINTER(ctypes.c_double))]

    
class _GphrxCsrAdjacencyMatrix_c(ctypes.Structure):
    _fields_ = [
        ("dimension", ctypes.c_uint64),
        ("col_indices", _DynamicArrayU64_c),
        ("row_indices", _DynamicArrayU64_c)]

    
class _GphrxCsrMatrix_c(ctypes.Structure):
    _fields_ = [
        ("dimension", ctypes.c_uint64),
        ("col_indices", _DynamicArrayDouble_c),
        ("col_indices", _DynamicArrayU64_c),
        ("row_indices", _DynamicArrayU64_c)]

    
class _GphrxGraph_c(ctypes.Structure):
    _fields_ = [
        ("is_undirected", ctypes.c_bool),
        ("adjacency_matrix", _GphrxCsrAdjacencyMatrix_c)]


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

_gphrx_lib.free_gphrx_csr_matrix.argtypes = [ctypes.POINTER(_GphrxCsrMatrix_c)]
_gphrx_lib.free_gphrx_csr_matrix.restype = None

_gphrx_lib.free_gphrx_csr_adj_matrix.argtypes = [ctypes.POINTER(_GphrxCsrAdjacencyMatrix_c)]
_gphrx_lib.free_gphrx_csr_adj_matrix.restype = None

_gphrx_lib.gphrx_csr_matrix_to_string.argtypes = (ctypes.POINTER(_GphrxCsrMatrix_c), ctypes.c_int)
_gphrx_lib.gphrx_csr_matrix_to_string.restype = ctypes.c_void_p

_gphrx_lib.gphrx_csr_adj_matrix_to_string.argtypes = [ctypes.POINTER(_GphrxCsrAdjacencyMatrix_c)]
_gphrx_lib.gphrx_csr_adj_matrix_to_string.restype = ctypes.c_void_p

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

_gphrx_lib.gphrx_find_occurrence_proportion_matrix.argtypes = (ctypes.POINTER(_GphrxGraph_c), ctypes.c_uint64)
_gphrx_lib.gphrx_find_occurrence_proportion_matrix.restype = _GphrxCsrMatrix_c

_gphrx_lib.approximate_gphrx.argtypes = (ctypes.POINTER(_GphrxGraph_c), ctypes.c_uint64, ctypes.c_double)
_gphrx_lib.approximate_gphrx.restype = _GphrxGraph_c

_gphrx_lib.gphrx_to_byte_array.argtypes = [ctypes.POINTER(_GphrxGraph_c)]
_gphrx_lib.gphrx_to_byte_array.restype = ctypes.POINTER(ctypes.c_ubyte)

_gphrx_lib.gphrx_from_byte_array.argtypes = [ctypes.POINTER(ctypes.c_ubyte)]
_gphrx_lib.gphrx_from_byte_array.restype = _GphrxGraph_c

_gphrx_lib.free_gphrx_byte_array.argtypes = [ctypes.c_void_p]
_gphrx_lib.free_gphrx_byte_array.restype = None


class GphrxOccurrenceProportionMatrix:
    def __init__(self, c_csr_matrix):
        self._matrix = c_csr_matrix

    def dimension(self):
        return self._matrix.dimension

    def to_string_with_precision(self, precision):
        c_str = _gphrx_lib.gphrx_csr_matrix_to_string(self._matrix, precision)
        py_str = ctypes.cast(c_str, ctypes.c_char_p).value
        _gphrx_lib.free_gphrx_byte_array(c_str)
        return py_str.decode('utf-8')

    def __str__(self):
        return self.to_string_with_precision(2)
    
    def __del__(self):
        _gphrx_lib.free_gphrx_csr_matrix(self._matrix)


class GphrxAdjacencyMatrix:
    def __init__(self, c_csr_adj_matrix):
        self._matrix = c_csr_adj_matrix

    def dimension(self):
        return self._matrix.dimension

    def __str__(self):
        c_str = _gphrx_lib.gphrx_csr_adj_matrix_to_string(self._matrix)
        py_str = ctypes.cast(c_str, ctypes.c_char_p).value
        _gphrx_lib.free_gphrx_byte_array(c_str)
        return py_str.decode('utf-8')
    
    def __del__(self):
        _gphrx_lib.free_gphrx_csr_adj_matrix(self._matrix)


class GphrxGraph:
    def __init__(self, is_undirected=True):
        self.is_undirected = is_undirected
        self._graph = _gphrx_lib.new_undirected_gphrx() if is_undirected else _gphrx_lib.new_directed_gphrx()
        self.adjacency_matrix = GphrxAdjacencyMatrix(self._graph.adjacency_matrix)

    def __del__(self):
        # Note: Freeing the GphrxGraph via free_gphrx will call free on the adjacency matrix. However, the
        #       adjacency matrix will already be freed when __del__ is called on self.adjacency_matrix
        # _gphrx_lib.free_gphrx(self._graph)
        pass

    def node_count(self):
        return self.adjacency_matrix.dimension()

    def edge_count(self):
        edges = self._graph.adjacency_matrix.col_indices.size 
        return int(edges / 2) if self.is_undirected else edges

    @staticmethod
    def from_bytes(byte_array):
        error_code = ctypes.c_uint8()
        arr = (ctypes.c_ubyte * (len(byte_array))).from_buffer(bytearray(byte_array))
        c_graph = _gphrx_lib.gphrx_from_byte_array(arr, ctypes.byref(error_code))

        if error_code.value != _GphrxErrorCode.GPHRX_NO_ERROR.value:
            raise ValueError("GrphxGraph could not be constructed from the provided bytes")

        graph = GphrxUndirectedGraph() if c_graph.is_undirected else GphrxDirectedGraph()
        
        _gphrx_lib.free_gphrx(graph._graph)
        graph._graph = c_graph
        graph.adjacency_matrix._matrix = c_graph.adjacency_matrix
        
        return graph

    def duplicate(self):
        c_graph = _gphrx_lib.duplicate_gphrx(self._graph)
        graph = GphrxUndirectedGraph() if c_graph.is_undirected else GphrxDirectedGraph()

        _gphrx_lib.free_gphrx(graph._graph)
        graph._graph = c_graph
        graph.adjacency_matrix._matrix = c_graph.adjacency_matrix

        return graph
        
    def shrink(self):
        _gphrx_lib.gphrx_shrink(self._graph)

    def does_edge_exist(self, from_vertex_id, to_vertex_id):
        return _gphrx_lib.gphrx_does_edge_exist(self._graph, from_vertex_id, to_vertex_id)

    def add_vertex(self, vertex_id, vertex_edges=[]):
        edges_arr = ctypes.c_uint64 * len(vertex_edges)
        _gphrx_lib.gphrx_add_vertex(self._graph, vertex_id, edges_arr(*vertex_edges), len(vertex_edges))

    def remove_vertex(self, vertex_id):
        _gphrx_lib.gphrx_remove_vertex(self._graph, vertex_id)

    def add_edge(self, from_vertex_id, to_vertex_id):
        _gphrx_lib.gphrx_add_edge(self._graph, from_vertex_id, to_vertex_id)

    def remove_edge(self, from_vertex_id, to_vertex_id):
        error_code = _gphrx_lib.gphrx_remove_edge(self._graph, from_vertex_id, to_vertex_id)

        if error_code == _GphrxErrorCode.GPHRX_ERROR_NOT_FOUND.value:
            raise ValueError("Edge from vertex " + str(from_vertex_id) +
                             " to vertex " + str(to_vertex_id) + " does not exist")

    def find_occurrence_proportion_matrix(self, block_dimension):
        c_matrix = _gphrx_lib.gphrx_find_occurrence_proportion_matrix(self._graph, block_dimension)
        return GphrxOccurrenceProportionMatrix(c_matrix)
        
    def approximate(self, block_dimension, threshold):
        c_graph = _gphrx_lib.approximate_gphrx(self._graph, block_dimension, threshold)
        graph = GphrxUndirectedGraph() if c_graph.is_undirected else GphrxDirectedGraph()

        graph._graph = c_graph
        graph.adjacency_matrix._matrix = c_graph.adjacency_matrix

        return graph

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
    test_graph = GphrxUndirectedGraph()
    
    print(test_graph.adjacency_matrix.dimension())
    print(test_graph.adjacency_matrix)
    
    test_graph.add_edge(4, 3)
    test_graph.add_edge(5, 5)
    test_graph.add_vertex(2, [1, 6, 7, 3, 0])
    test_graph.add_vertex(8)
    test_graph = test_graph.duplicate()
    test_graph.add_edge(0, 1)
    print(test_graph.does_edge_exist(3, 4))
    print()
    
    # test_graph.save_to_file('./test_graph.gphrx')

    print(test_graph.adjacency_matrix)
    print()
    
    print(test_graph.approximate(3, 0.3).adjacency_matrix)
    print()
    
    test_graph.remove_edge(7, 2)
    test_graph.remove_vertex(2)
    test_graph.shrink()
    test_graph.remove_edge(4, 3)
    
    print(test_graph.adjacency_matrix)

    test_graph2 = GphrxGraph.from_bytes(bytes(test_graph))
