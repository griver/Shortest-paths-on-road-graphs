#ifndef FULL_ACCESS_NODES_CHECKERS
#define FULL_ACCESS_NODES_CHECKERS


#include "tnr_checkers.h"

namespace tnr {
	bool an_checking_dijkstra(request_data_t data, shared_ptr<access_map> access_nodes) {
		base_algorithm::run_dijkstra(data.graph, data.start_vertex, data.end_vertex, data.visited1, data.visited2, data.shortest_path);
		
		vertex_id start = data.start_vertex;
		vertex_id end = data.end_vertex;
		path_map& path = *(data.shortest_path);
		
		if(path.size() == 0)
			return true;
		
		path_vertex current = path[end];
		vertex_id current_id = end;
		
		int start_an_index = -1;
		int end_an_index = -1;
		int index = 0;

		while(current.inc.is_initialized()) {
			if(current_id != end && current_id != start) {
				if((*access_nodes)[end].count(current_id) != 0) {
					cout << "end access node: " << current_id << endl;
					end_an_index = index;
				}
				if((*access_nodes)[start].count(current_id) != 0) {
					cout << "start access node: " << current_id << endl;
					start_an_index = index;	
				}

			}
			++index;
			current_id = (*current.parent);
			current = path[(*current.parent)];
		}
		
		if(end_an_index != -1 && start_an_index != -1 && start_an_index >= end_an_index)
			return true;
		else
			return false;
	}



	void check_access_nodes(request_data_t data, shared_ptr<access_map> access_nodes) {
		grid_info info(data.graph, 100, 100);
		srand ( time(NULL) );
		int index = 0;
		while(index < 10000) {
			data.shortest_path->clear();
			data.visited1->clear();
			data.visited2->clear();
			data.start_vertex = rand() % (data.graph.v_count()-1) + 1;
			data.end_vertex = rand() % (data.graph.v_count()-1) + 1;
			coord_t const &start_c = data.graph.get_vertex(data.start_vertex).data.c;
			coord_t const &end_c = data.graph.get_vertex(data.end_vertex).data.c;
			grid_cell start_cell = info.get_cell(start_c);
			grid_cell end_cell = info.get_cell(end_c);

			if(std::abs(start_cell.x - end_cell.x) > 4) {
				index++;
				bool result = an_checking_dijkstra(data, access_nodes);
				print_result(result, data.start_vertex, data.end_vertex, index);
				if(!result) return;
			}
			else if(std::abs(start_cell.y - end_cell.y) > 4) {
				index++;
				bool result = an_checking_dijkstra(data, access_nodes);
				print_result(result, data.start_vertex, data.end_vertex, index);
				if(!result) return;
			}
		}
	}

}

#endif