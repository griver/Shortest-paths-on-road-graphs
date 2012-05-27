#ifndef TNR_CHECKERS
#define TNR_CHECKERS



#include "../../tnr_utils/grid_info.h"
#include "../preprocessing/transit_node_filter.h"
#include "../preprocessing/node_selector.h"
#include "../../tnr_utils/tnr_utils.h"
#include "../base_algorithm/request_data.h"
#include "../base_algorithm/run_algorithms.h" 
#include "../tnr_path_finder.h"

#include <cstdlib>
#include <ctime>
#include <cfloat>




namespace tnr {
	void print_result(bool result, vertex_id start, vertex_id end, int index) {
		std::cerr << index << std::endl;
		std::cerr << "path from: " << start << " to: " << end;
		if(result == false) {
			std::cerr << " :     PROBLEM!!!!" << std::endl;	
		} else {
			std::cerr << " :     all correct" << std::endl;
		}
	}




	void an_graph_check(shared_ptr<access_map> an, shared_ptr<vertex_set>  tn) {
		cout << "an_graph_check " << endl;	
		vertex_set::const_iterator iter = (*tn).begin();
		vertex_set::const_iterator end_iter = (*tn).end();
		int count_e, count_v, edge_number, null_count;
		count_v = 0;
		count_e = 0;
		edge_number = 0;
		null_count = 0;
		edge_weight min_weight = FLT_MAX;
		edge_weight max_diff = 0.0f;
		for(iter; iter != end_iter; ++iter) {
			vertex_id t_id= (*iter);
			
			unordered_map<vertex_id, access_info>::const_iterator an_iter = (*an)[t_id].begin();
			unordered_map<vertex_id, access_info>::const_iterator an_end_iter = (*an)[t_id].end();

			for(an_iter; an_iter != an_end_iter; ++an_iter) {
				
				vertex_id an_id = an_iter->first;
				edge_weight an_dist = an_iter->second.dist;
				edge_number++;
				if((*an)[an_id].count(t_id) == 0) {
					count_v++;
					//cout << "t_id: " << t_id << " have " << an_id << " ass access node but " <<t_id << " not an access node of " << an_id << endl;
					//return ;
				}
				if((*an)[an_id][t_id].dist != an_dist) {
					count_e++;
					if(max_diff < abs(an_dist - (*an)[an_id][t_id].dist))
						max_diff = abs(an_dist - (*an)[an_id][t_id].dist);
					//cout << an_dist << " " << (*an)[an_id][t_id] << endl

					//return;
				}
				if(an_dist == 0.0f) {
					null_count++;
					cout << t_id << "  " << an_id <<endl;
				}
				if(an_dist < min_weight) min_weight = an_dist;
			}
		}
		cout <<  "min_dist = " << min_weight << endl;
		cout << "e errors =  " << count_e << endl;
		cout << "v errors =  " << count_v << endl;
		cout << "edge_number = " << edge_number << endl;
		cout << "max_diff = " << max_diff << endl;
		cout << "null_count = " << null_count << endl; 
	}


	bool compare_paths(path_map* a, path_map* b) {
		path_map::const_iterator iter_a = a->begin();
		path_map::const_iterator end_a = a->end();
		edge_id e_a, e_b;
		vertex_id parent_a, parent_b;

		if(a->size() != b->size()) {
			cout << "(a("<< a->size() <<") != b("<< b->size() <<"))" << endl;

			return false;
		}

		for(iter_a; iter_a != end_a; ++iter_a) {
			vertex_id v = iter_a->first;
			if(b->count(v) == 0) {
				cout << "(b->count(v) == 0)" << endl;
				return false;
			}

			e_a = *(iter_a->second.inc);
			e_b = *((*b)[v].inc);

			if(e_b != e_a) {
				cout << "(e_b != e_a)" << endl;
				return false;
			}
			
			parent_a = *(iter_a->second.parent); 
			parent_b = *((*b)[v].parent);

			if(parent_a != parent_b) {
				cout << "(parent_a != parent_b)" << endl;
				return false;
			}
		}

		return true;
	}
	
	bool compare_paths_dist(path_map* a, path_map* b, vertex_id target) {
		if(((*a)[target].d - (*b)[target].d) > 0.00001f ) {
			cout << (*a)[target].d - (*b)[target].d << " != " << 0.0f << endl;
			return false;
		}
		return true;
	}

	void count_paths_edges(path_map* a, graph_t const & graph, int & edges_a, int & null_a) {
		path_map::const_iterator iter_a = a->begin();
		path_map::const_iterator end_a = a->end();
		
		edges_a = 0;
		null_a = 0;
		for(iter_a; iter_a != end_a; ++iter_a) {
			edge_id edge = *(iter_a->second.inc);
			if(graph.get_edge(edge).data.len == 0.0f)
				null_a++;
			else 
				edges_a++;
		}
	}

	bool compare_paths_edges(path_map* a, path_map* b, graph_t const & graph) {
		int a_edges, b_edges;
		int	a_nulls, b_nulls;
		count_paths_edges(a, graph, a_edges, a_nulls);
		count_paths_edges(b, graph, b_edges, b_nulls);
		if(a_edges != b_edges)
			return false;
		else 
			return true;
	}

	void check_tnr_path_finder(request_data_t data, tnr_path_finder& finder) {
		grid_info info(data.graph, 100, 100);
		srand ( time(NULL) );
		int index = 0;
		path_map* d_path = new path_map();

		while(index < 10000) {
			data.shortest_path->clear();
			data.visited1->clear();
			d_path->clear();

			data.start_vertex = rand() % (data.graph.v_count()-1) + 1;
			data.end_vertex = rand() % (data.graph.v_count()-1) + 1;
			coord_t const &start_c = data.graph.get_vertex(data.start_vertex).data.c;
			coord_t const &end_c = data.graph.get_vertex(data.end_vertex).data.c;
			grid_cell start_cell = info.get_cell(start_c);
			grid_cell end_cell = info.get_cell(end_c);
			bool far_enough = false;
			if(std::abs(start_cell.x - end_cell.x) > 4)
				far_enough = true;
			else if(std::abs(start_cell.y - end_cell.y) > 4) 
				far_enough = true;

			if(far_enough) {
				index++;
				finder.search(data.start_vertex, data.end_vertex);
				run_astar(data.graph, data.start_vertex, data.end_vertex, data.visited1, data.visited2, d_path);
				//bool result = compare_paths(d_path, data.shortest_path);
				bool result = compare_paths_dist(d_path, data.shortest_path, data.end_vertex);
				print_result(result, data.start_vertex, data.end_vertex, index);
				
				if(!result) return;
			}
		}
	}
}

#endif