from ctypes import cdll

#search = ctypes.CDLL("/home/ivanovpc/workspace/frontend/searchlib.so") #.pyd files

lib = cdll.LoadLibrary("/home/ivanovak/workspace/frontend/lib/_libroute.so")

lib.get_path()


