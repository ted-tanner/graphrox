from pathlib import Path

import ctypes
import os
import platform


__libs_path = os.path.join(Path(__file__).resolve().parent, 'lib')

__os_name = platform.system()

if __os_name == 'Linux':
    __dll_name = 'graphrox.so'
elif __os_name == 'Windows':
    __dll_name = 'graphrox.dll'
elif __os_name == 'Darwin':
    if 'x86_64' == platform.uname().machine:
        __dll_name = 'graphrox-x86.dylib'
    else:
        __dll_name = 'graphrox-arm64.dylib'
else:
    raise ImportError('GraphRox module not supported on this system')


class DynamicArrayU64(ctypes.Structure):
    _fields_ = [
        ("capacity", ctypes.c_size_t),
        ("size", ctypes.c_size_t),
        ("arr", ctypes.POINTER(ctypes.c_uint64))]

class CsrAdjMatrix(ctypes.Structure):
    _fields_ = [
        ("matrix_col_idx_list", DynamicArrayU64),
        ("matrix_row_idx_list", DynamicArrayU64)]

class GphrxGraph(ctypes.Structure):
    _fields_ = [
        ("is_undirected", ctypes.c_bool),
        ("highest_vertex_id", ctypes.c_uint64),
        ("adjacency_matrix", CsrAdjMatrix)]
    

__gphrx_lib_path = os.path.join(__libs_path, __dll_name)
__gphrx_lib = ctypes.cdll.LoadLibrary(__gphrx_lib_path)

__gphrx_lib.new_undirected_gphrx.argtypes = None
__gphrx_lib.new_undirected_gphrx.restype = GphrxGraph

__gphrx_lib.free_gphrx.argtypes = [ctypes.POINTER(GphrxGraph)]
__gphrx_lib.free_gphrx.restype = None

test = __gphrx_lib.new_undirected_gphrx()

print(test)

__gphrx_lib.free_gphrx(test)



# __gphrx_lib.csv_to_json.argtypes = (ctypes.c_char_p, ctypes.c_size_t)
# __gphrx_lib.csv_to_json.restype = ctypes.c_void_p

# __gphrx_lib.free_json.argtypes = [ctypes.c_void_p]
# __gphrx_lib.free_json.restype = None


# class __DynamicArray64(ctypes.Structure):
#     _fields_ = [('capacity', ctypes.c_size_t),
#                 ('size', ctypes.c_size_t),
#                 ('arr', c_void_p)]

    
# class __CsrAdjMatrix(ctypes.Structure):
#     _fields_ = [('matrix_col_idx_list', c_int),
#                 ('matrix_row_idx_list', c_double)]
    

# class GphrxGraph(ctypes.Structure):
#     _fields_ = [('num', c_int),
#                 ('dnum', c_double)]


# def csv_to_json(csv, as_str=False):
#     if isinstance(csv, str):
#         csv = bytes(csv, encoding='utf-8')

#     ptr = __c_csv_lib.csv_to_json(csv, len(csv))
#     json = ctypes.cast(ptr, ctypes.c_char_p).value

#     __c_csv_lib.free_json(ptr)

#     if as_str:
#         return json.decode('utf-8')
#     else:
#         return json
