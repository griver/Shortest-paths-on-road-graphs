from ctypes import *

#cdll.LoadLibrary("/home/ivanovak/workspace/Practice/frontend/lib/new/example.so") 
libc = CDLL("/home/ivanovak/workspace/Practice/frontend/lib/new/example.so")     

libc.get_i.restype = c_float;
libc.get_path.restype = c_void_p
libc.search.argtypes = [c_float, c_float, c_float, c_float]

#ptr = POINTER(c_float)()
libc.search(30., 60., 31., 61.)

print libc.get_i(1);
print libc.get_i(2);
ptr = libc.get_path()
print type(ptr)


#libc.free_mem()
#ptr = libc.get_path()


