#ifndef TNR_UTILS_H
#define TNR_UTILS_H

#include "stdafx.h"

#include <cmath>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <cfloat>

#include "mapping_exception.h"
#include "path_finder.h"
#include "grid_cell.h"


namespace tnr {

	struct full_access_info {
		size_t next;
		my_graph::edge_weight dist; 
		full_access_info(): next(0), dist(0.0f) {}
		full_access_info(size_t next , my_graph::edge_weight dist): next(next), dist(dist) {}
	};

	struct access_info {
		my_graph::vertex_id next;
		my_graph::edge_weight dist; 
		access_info(): next(0), dist(0.0f) {}
		access_info(my_graph::vertex_id next , my_graph::edge_weight dist): next(next), dist(dist) {}
	};
	
	bool operator<(access_info const & a, access_info const & b) {
		return a.dist < b.dist;
	}

	using namespace my_algorithm;
	using std::deque;

	typedef std::set<vertex_id> vertex_set;
	typedef coord<double> grid_coord;
	typedef std::map<grid_cell, vertex_set> border_map;
	typedef unordered_map<vertex_id, unordered_map<vertex_id, access_info> > access_map;
	typedef vector<unordered_map<vertex_id, full_access_info> > full_access_map;

	//class dist_table;
}



#endif