#ifndef TNR_ALGORITHMS_H
#define	TNR_ALGORITHMS_H

#include "path_finder.h"
#include "grid_info.h"
#include "coordinate_filter.h"

namespace TNR {
	
	void search_in_rectangle(const graph_t &graph,
					   vertex_id start_vertex,
					   vertex_id end_vertex,
					   path_map *visited1, 
					   path_map *visited2, 
					   path_map *shortest_path) {
		
		TNR::grid_info info(graph, shortest_path, 100, 100);
		path_finder forward(&graph, get_weight, visited1);
		
		grid_cell cell = info.get_cell(graph.get_vertex(start_vertex).data.c);
		coordinate_filter::check_function fn = bind(&grid_info::is_in_rectangle, &info, cell.x-5, cell.y-5, cell.x+4, cell.y+4, _1);
		coordinate_filter* filter = new coordinate_filter("RectangleFilter", fn);
		shared_ptr<queue_filter> checker(filter);
	
		forward.set_priority_queue_filter(checker);
	
		forward.init_search(start_vertex, end_vertex);		
		while(filter->get_counter() > 0) {
			if(forward.is_intersect()) break;
			forward.iterate();
		}	
	}
}

#endif