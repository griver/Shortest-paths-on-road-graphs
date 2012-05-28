#include "stdafx.h"
#include "routes.h"

scoped_ptr<routes> g_proutes;
vector<vis_coord> g_orig_path;
vector<float> g_path;

extern "C" {
	void free_mem();
	float get_i(int);
	float get_access_i(int);
	int get_access_size();
	void myinit();
	int get_size();
	float * get_path();
	void mysearch(float x1, float y1, float x2, float y2);
}

void myinit() 
{
	std::cout << "Loading OSM map file..." << std::endl;
	//g_proutes.reset(new routes("/home/ivanovpc/workspace/server2/map/city_of_st_petersburg.highway.osm"));
    g_proutes.reset(new routes("/home/ivanovak/workspace/Practice/Shortest-paths-on-road-graphs/osm_maps/ireland.osm"));
}

float *get_path() 
{
	return &(g_path[0]);
}

void mysearch(float x1, float y1, float x2, float y2) {

	if (g_proutes == NULL) {	   
		return;
	}
	
	//std::cout << "I see routes..." << std::endl;
	const vertex_id start = g_proutes->get_vertex_by_coord(vis_coord(x1, y1));
	const vertex_id end = g_proutes->get_vertex_by_coord(vis_coord(x2, y2));
	g_orig_path.clear();
	g_path.clear();
	//std::cout << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;	
	g_proutes->get_path(start, end, g_orig_path);

	g_path.resize(g_orig_path.size() * 2);
	//std::cout << "starting giving " << g_orig_path.size() << " of them..."<< std::endl;
	//std::cout << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
	//std::cout << start << " " << end << std::endl;
	for (size_t i = 0; i < g_orig_path.size(); ++i)
	{
	    g_path[i * 2 + 0] = static_cast<float>(g_orig_path[i].x);
	    g_path[i * 2 + 1] = static_cast<float>(g_orig_path[i].y);
		//std::cout << static_cast<float>(g_orig_path[i].x) << " " <<
		//			 static_cast<float>(g_orig_path[i].y) << std::endl;
	}

}

int get_size() 
{
	return g_path.size();
}

float get_i(int i)
{
	return g_path[i];
}

float get_access_i(int i)
{
	return 0.;
}

int get_access_size()
{
	return 0;
}

void free_mem()
{
	g_path.clear();
}













