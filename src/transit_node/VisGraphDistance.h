#ifndef VIS_GRAPH_DISTANCE_H
#define VIS_GRAPH_DISTANCE_H
#include "stdafx.h"
#include "../shared/new_vis_graph.h"
#include "../shared/coord.h"
#include <cmath>

namespace my_algorithm {
	typedef vertex_base<vis_vertex_data, vis_edge_data> VisVertex;

	inline edge_weight visGraphDistance(VisVertex const & source, VisVertex const & target) {
		float sX = source.get_data().c.x;
		float sY = source.get_data().c.y;

		float tX = target.get_data().c.x;
		float tY = target.get_data().c.y;
				 
		return sqrt(pow(abs(tX-sX), 2)+ pow(abs(tY-sY), 2));
	}

	inline edge_weight defaultEstimate(VisVertex const & source, VisVertex const & target) {
		return 0.0f;
	}

}
#endif
