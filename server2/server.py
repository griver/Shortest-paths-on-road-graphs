import json
import web
from ctypes import *

libc = CDLL("/home/ivanovpc/workspace/server2/lib/libroute.so")     

libc.get_i.restype = c_float;
libc.get_path.restype = c_void_p
libc.search.argtypes = [c_float, c_float, c_float, c_float]

def build_list(nitems):
	l = []
	for i in range(0, nitems):
		l.append(libc.get_i(i))
	return l

#libc.search(30., 60., 31., 61.)

render = web.template.render('templates/')

#web.config.debug = False


urls = (
  '/', 'index'
)

'''
urls = (
  '/(.*)', 'index'
)
'''


class index:
	def GET(self):
		json_par = 0
		i = web.input(json_par=None)

		#print(i.lon1)
		#print(i.lat2)
		
		"""
			Here comes a map page request (without a route)
		"""

		if (i.json_par is None):
			web.header('Content-Type', 'text/html')
			return render.map()
	
		"""
			Processing the route right here
		"""
		#myPoints = {'arr': }
        #web.header('Content-Type', 'application/json')
        #data = [30, 60, 31, 61, 32, 59]
		ll = json.loads(i.json_par)
		libc.search(ll["lon1"], ll["lat1"], ll["lon2"], ll["lat2"])		
		data = build_list(libc.get_size())
		libc.free_mem()
		return json.dumps({ "array": data })
		#return render.route(i.lon1, i.lat1, i.lon1 + 500, i.lat1 + 1000)

		#print type(i.lon1)
		#print type(c_float(i.lon1))
		#print c_float(i.lon1)
		#return libc.search(c_float(i.lon1), c_float(i.lat1), c_float(i.lon2), c_float(i.lat2)) 
		# pathArray = search(i.lon1, i.lat1, i.lon2, i.lat2)
		# return render.route(points)
		
		#pathArray = [i.lon1, i.lat1, i.lon1 + 500, i.lat1 + 1000, i.lon2, i.lat2]
		
		#web.header('Content-Type', 'application/json')
		#return json.dumps(pathArray)

	#def GET(self, name):
	#	return render.index(name)

if __name__ == "__main__": 
	app = web.application(urls, globals())
	app.run()   
