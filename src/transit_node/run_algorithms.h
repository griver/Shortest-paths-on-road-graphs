#ifndef RUN_ALGORITHMS_H
#define RUN_ALGORITHMS_H

#include "path_finder.h"

namespace my_algorithm {
	
	void run_astar(const graph_t &graph,
					   vertex_id start_vertex,
					   vertex_id end_vertex,
					   path_map *visited1, 
					   path_map *visited2, 
					   path_map *shortest_path) {
		// поиск кратчайшего растояния до вершины
			
		path_finder forward(&graph, get_weight, vis_graph_distance, visited1);
		
		std::set<edge_id> border_edges;
		forward.init_search(start_vertex, end_vertex, false, visited2, &border_edges);
		
		while(!forward.is_stopped() && !forward.is_find()) {
			if(forward.is_intersect()) break;
			forward.iterate();
		}
		
		if(!forward.is_find())
			return;

		path_vertex current = (*visited1)[end_vertex];
		vertex_id current_id = end_vertex;

		while(current.inc.is_initialized()) {
			(*shortest_path)[current_id] = current;
			current_id = (*current.parent);
			current = (*visited1)[(*current.parent)];
		}
		(*shortest_path)[start_vertex] = (*visited1)[start_vertex];
	}

	void run_dijkstra(const graph_t &graph,
					   vertex_id start_vertex,
					   vertex_id end_vertex,
					   path_map *visited1, 
					   path_map *visited2, 
					   path_map *shortest_path) {
		// поиск кратчайшего растояния до вершины
			
		path_finder forward(&graph, get_weight, visited1);
		
		std::set<edge_id> border_edges;
		forward.init_search(start_vertex, end_vertex, false, visited2, &border_edges);
		
		while(!forward.is_stopped() && !forward.is_find()) {
			if(forward.is_intersect()) break;
			forward.iterate();
		}
		
		if(!forward.is_find())
			return;

		path_vertex current = (*visited1)[end_vertex];
		vertex_id current_id = end_vertex;

		while(current.inc.is_initialized()) {
			(*shortest_path)[current_id] = current;
			current_id = (*current.parent);
			current = (*visited1)[(*current.parent)];
		}
		(*shortest_path)[start_vertex] = (*visited1)[start_vertex];
	}
}

#endif
