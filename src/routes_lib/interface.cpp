#include "stdafx.h"
#include "routes.h"

scoped_ptr<routes> g_proutes;
vector<vis_coord> g_orig_path;
vector<float> g_path;

extern "C" {
	void free_mem();
	float get_i(int i);
	void myinit();
	int get_size();
	float * get_path();
	void search(float x1, float y1, float x2, float y2);
}

void myinit() 
{
	g_proutes.reset(new routes("/home/ivanovpc/workspace/server2/map/city_of_st_petersburg.highway.osm"));
}

float *get_path() 
{
	return &(g_path[0]);
}

void search(float x1, float y1, float x2, float y2)
{
	if (g_proutes == NULL)
	    return;

	const vertex_id start = g_proutes->get_vertex_by_coord(vis_coord(x1, y1));
	const vertex_id end = g_proutes->get_vertex_by_coord(vis_coord(x1, y1));
	
	g_proutes->get_path(start, end, g_orig_path);

	g_path.resize(g_orig_path.size() * 2);
	for (size_t i = 0; i < g_orig_path.size(); ++i)
	{
	    g_path[i * 2 + 0] = static_cast<float>(g_orig_path[i].x);
	    g_path[i * 2 + 1] = static_cast<float>(g_orig_path[i].y);
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

void free_mem()
{
	g_path.clear();
}













