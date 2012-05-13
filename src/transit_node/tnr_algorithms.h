#ifndef TNR_ALGORITHMS_H
#define	TNR_ALGORITHMS_H

#include "path_finder.h"
#include "grid_info.h"
#include "coordinate_filter.h"
#include "transit_node_filter.h"
#include "node_selector.h"
#include "tnr_utils.h"
#include <fstream>

namespace tnr {

	inline void calculate_cell_access_nodes(grid_cell const & cell,
											vertex_set const & border_nodes,
											path_finder &finder,
											node_selector& selector,
											vertex_set & transit_nodes,
											grid_info & info,
											path_map* visited_tree) {
		cout << "---------------------------------------------------------------------------" << endl;
		cout << "process cell:" << cell.x << "  " << cell.y << endl;
		cout << "border_set size: " << border_nodes.size() << endl;
		size_t old_size = transit_nodes.size();
		vertex_set::const_iterator iter = border_nodes.begin();
		vertex_set::const_iterator end = border_nodes.end();

		coordinate_check in_outer = bind(&grid_info::is_in_rectangle, &info, cell.x-5, cell.y-5, cell.x+4, cell.y+4, _1);
		coordinate_check in_inner = bind(&grid_info::is_in_rectangle, &info, cell.x-3, cell.y-3, cell.x+2, cell.y+2, _1);
		coordinate_filter* filter = new coordinate_filter("RectangleFilter", in_outer);
		shared_ptr<queue_filter> checker(filter);

		finder.set_priority_queue_filter(checker);
		selector.set_coordinate_checkers(in_inner, in_outer);
	
		for(iter; iter != end; ++iter) {
			finder.init_search( (*iter), (*iter));
			while(filter->get_counter() > 0) {
				if(finder.is_intersect()) break;
				finder.iterate();
			}	 
			//cout<<"visited_tree size is: " << visited_tree->size() << "  and root is: " << *iter  << endl; 
			vertex_set const & access_nodes = selector.select(visited_tree, *iter);
			for(vertex_set:: const_iterator i = access_nodes.begin(); i != access_nodes.end(); ++i) {
				transit_nodes.insert((*i));
			}
		}
		cout << "transit_nodes size: " << transit_nodes.size()<< endl;
	}
	
	void search_in_rectangle(const graph_t &graph,
					   vertex_id start_vertex,
					   vertex_id end_vertex,
					   path_map *visited_tree, 
					   path_map *shortest_path) {
		vertex_set transit_nodes;	
		tnr::grid_info info(graph, shortest_path, 100, 100);
		
		path_finder forward(&graph, get_weight, visited_tree);
		
		grid_cell cell = info.get_cell(graph.get_vertex(start_vertex).data.c);
		coordinate_check in_outer = bind(&grid_info::is_in_rectangle, &info, cell.x-5, cell.y-5, cell.x+4, cell.y+4, _1);
		coordinate_check in_inner = bind(&grid_info::is_in_rectangle, &info, cell.x-3, cell.y-3, cell.x+2, cell.y+2, _1);
		coordinate_filter* filter = new coordinate_filter("RectangleFilter", in_outer);
		shared_ptr<queue_filter> checker(filter);
	
		forward.set_priority_queue_filter(checker);
	
		forward.init_search(start_vertex, start_vertex);		
		while(filter->get_counter() > 0) {
			if(forward.is_intersect()) break;
			forward.iterate();
		}	
		node_selector selector(graph, in_inner, in_outer);
		vertex_set const & access_nodes = selector.select(visited_tree, start_vertex);
		cout << endl << "///////Number of vertex visited buy tree:  " <<visited_tree->size() << endl;
		cout << "///////Number of selected access nodes: " <<access_nodes.size() << endl;
	}


	
	void grid_based_preprocessor(const graph_t &graph,
					   path_map *visited_tree, 
					   path_map *shortest_path) {
		grid_info info(graph, shortest_path, 100, 100);
		vertex_set transit_nodes;

		border_map & grid_borders = info.get_border_map();
		border_map::iterator iter = grid_borders.begin();
		border_map::iterator end = grid_borders.end();
		
		path_finder finder(&graph, get_weight, visited_tree);
		node_selector selector(graph);
		

		for(iter; iter != end; ++iter) {
			//if(iter->first.x == 49 && iter->first.y == 89)
			calculate_cell_access_nodes(iter->first, iter->second, finder, selector, transit_nodes, info, visited_tree);
		}

		cout << "RESULT SIZE: " << transit_nodes.size();

		std::ofstream outfile ("ireland.tn");
		for(vertex_set::iterator i = transit_nodes.begin(); i != transit_nodes.end(); ++i)
			outfile << (*i) << endl;
		outfile.close();
	}





	void calculate_access_nodes(const graph_t &graph, path_map *visited_tree, shared_ptr<vertex_set>& transit_nodes) {
		
		access_map access_nodes;
		path_finder finder(&graph, get_weight, visited_tree);		
		
		for(vertex_set::iterator iter = transit_nodes->begin(); iter != transit_nodes->end(); ++iter) {

			vertex_id start_id = *iter;
			//cout<< "2" <<endl;
			transit_node_filter* tn_filter = new transit_node_filter("transit node filter", transit_nodes, (*iter));
			//cout<< "3" <<endl;
			shared_ptr<queue_filter> checker(tn_filter);
			//cout<< "4" <<endl;
			finder.set_priority_queue_filter(checker);
			//cout<< "5" <<endl;
			finder.init_search(*iter, *iter);
			//cout<< "6" <<endl;
			while(tn_filter->get_node_before() > 0) {
				if(finder.is_intersect()) break;
				finder.iterate();
			} //while
			
			unordered_map<vertex_id, reached_state>const & reached = tn_filter->get_when_reached_map();
			unordered_map<vertex_id, reached_state>::const_iterator it = reached.begin();
			unordered_map<vertex_id, reached_state>::const_iterator end_it = reached.end();
			int i = 0;
			for(it; it != end_it; ++it) {
				vertex_id v_id = it->first;
				reached_state v_state = it->second; 	
				if(v_state == BEFORE_TNR) {
					if((*visited_tree).count(v_id) == 0) {
						cout <<"PROBLEM!!!!" << endl;
						return;
					}
					//access_nodes[v_id].insert(access_node_info(start_id, (*visited_tree)[v_id].d));
					if(access_nodes.count(v_id) != 0) {
						if(access_nodes[v_id].count(start_id) != 0) {
							cout <<"OTHER PROBLEM!!!!" << endl;
							return;
						}
					}
					if(v_id != start_id) {
						vertex_id next = *((*visited_tree)[v_id].parent);
						edge_weight dist = (*visited_tree)[v_id].d;
						(access_nodes[v_id])[start_id] = access_info(next, dist);
		
						if(transit_nodes->count(it->first) > 0) i++;
					} // (v_id != start_id)
				}
			}
			cout << "root = " << *iter << " has " << i << " access nodes" << endl;
		} //for	
		//записать в файл
		std::ofstream outfile ("ireland.an");
		int count =0;
		for(access_map::iterator am_iter = access_nodes.begin(); am_iter != access_nodes.end(); ++am_iter) {
			vertex_id v_id = am_iter->first;
			//if(transit_nodes->count(v_id) == 0)
			//	continue;
			unordered_map<vertex_id, access_info>& v_map = am_iter->second;
			unordered_map<vertex_id, access_info>::const_iterator an_iter = v_map.begin();
			unordered_map<vertex_id, access_info>::const_iterator an_iter_end = v_map.end();
			outfile << "vertex: " << v_id << endl;
			outfile << "size: " << v_map.size() << endl;
			for(an_iter; an_iter != an_iter_end; ++an_iter) {
				vertex_id access_id = an_iter->first;
				edge_weight access_dist = an_iter->second.dist;
				vertex_id access_next = an_iter->second.next;
				outfile << access_id << " " << access_dist << " " << access_next << endl;  
			}
		}

		outfile.close();
		cout << "DONE!!" << endl;

	} //transit_node_dijkstra


	//void calculate_distance_table()
	
}//namespace tnr {


#endif