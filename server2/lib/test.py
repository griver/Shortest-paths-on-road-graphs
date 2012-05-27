from ctypes import *


libc = CDLL("/home/ivanovak/workspace/Practice/frontend/lib/libroute.so")     

libc.get_i.restype = c_float;
libc.get_path.restype = c_void_p
libc.search.argtypes = [c_float, c_float, c_float, c_float]

def build_list(nitems):
	l = []
	for i in range(0, nitems):
		l.append(libc.get_i(i))
	return l

libc.search(30, 60, 31, 61);
print build_list(libc.get_size())


