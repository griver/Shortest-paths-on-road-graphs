#ifndef GRAPH_UTILS_H
#define GRAPH_UTILS_H
#include "stdafx.h"
#include "../shared/new_vis_graph.h"
#include "../shared/coord.h"
#include "../shared/common.h"


#include <cmath>

namespace my_algorithm {
	using namespace my_graph;

	typedef vis_vertex vertex_t;
	typedef vis_graph graph_t;
	typedef vis_edge edge_t;
	typedef vis_coord coord_t;
	typedef boost::function<bool (coord_t const &)> coordinate_check;

	inline edge_weight vis_graph_distance(vertex_t const & source, vertex_t const & target) {
		float sX = source.get_data().c.x;
		float sY = source.get_data().c.y;

		float tX = target.get_data().c.x;
		float tY = target.get_data().c.y;
				 
		return sqrt(pow(abs(tX-sX), 2)+ pow(abs(tY-sY), 2));
	}

	inline edge_weight default_estimate(vertex_t const & source, vertex_t const & target) {
		return 0.0f;
	}

	inline edge_weight  get_weight(const edge_t &e) {return e.data.len;}

}
#endif
