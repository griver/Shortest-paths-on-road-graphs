#ifndef REQUEST_DATA_H
#define REQUEST_DATA_H

#include "../shared/new_vis_graph.h"
#include "../shared/coord.h"
#include "../shared/common.h"

namespace base_algorithm {
	struct request_data_t {
		const graph_t &graph;
		vertex_id start_vertex;
		vertex_id end_vertex;
		path_map *visited1; 
		path_map *visited2; 
		path_map *shortest_path;
		request_data_t(const graph_t &graph,
					   vertex_id start_vertex = 0,
					   vertex_id end_vertex = 0,
					   path_map *visited1 = 0, 
					   path_map *visited2 = 0, 
					   path_map *shortest_path = 0)
					   :graph(graph),
						start_vertex(start_vertex),
						end_vertex(end_vertex),
						visited1(visited1),
						visited2(visited2),
						shortest_path(shortest_path){}
	};
}

#endif