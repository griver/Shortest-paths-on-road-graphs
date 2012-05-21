#ifndef ACCESS_NODE_PREPROCESSOR_H
#define ACCESS_NODE_PREPROCESSOR_H

#include "tnr_utils.h"
#include "transit_node_filter.h"

namespace tnr {

	size_t get_next_index(vertex_id id, vertex_id next, graph_t const &graph ) {
		vertex_t::adj_iterator iter = graph.get_vertex(id).out_begin();
		vertex_t::adj_iterator end = graph.get_vertex(id).out_end();
		for(iter; iter != end; ++iter) {
			if(iter->v == next)
				return (iter - graph.get_vertex(id).out_begin());
		}
	}

	void calculate_access_nodes(const graph_t &graph, path_map *visited_tree, shared_ptr<vertex_set>& transit_nodes, string const & output) {
		
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
		std::ofstream outfile (output.c_str());
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

	}

	void calculate_full_access_nodes(const graph_t &graph, path_map *visited_tree, shared_ptr<vertex_set>& transit_nodes, string const & output) {
		int access_number = 0;
		int root_number = 0;
		full_access_map access_nodes;
		access_nodes.resize(graph.v_count()+1);
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
					
					if(access_nodes[v_id].count(start_id) != 0) {
						cout <<"OTHER PROBLEM!!!!" << endl;
						return;
					}
					

					if(v_id != start_id) {
						vertex_id next = *((*visited_tree)[v_id].parent);
						size_t  next_index = get_next_index(v_id, next, graph);
						edge_weight dist = (*visited_tree)[v_id].d;

						(access_nodes[v_id])[start_id] = full_access_info(next_index, dist);

						if(transit_nodes->count(it->first) > 0) i++;
					} // (v_id != start_id)
				}
			} //for(it; it != end_it; ++it)
			cout << "root = " << *iter << " has " << i << " access nodes" << endl;
			access_number += i;
			root_number++;
		} //for

		cout << "roots: " << root_number << endl << "access nodes:" << access_number << endl;
		cout << "mean value: " << access_number / root_number << endl;
		root_number = access_number = 0;
		//записать в файл
		std::ofstream outfile (output.c_str());
		outfile << "access_map_size: " << access_nodes.size() << endl;
		for(int i = 0; i < access_nodes.size(); ++i) {
			vertex_id v_id = (vertex_id)i;
			if(transit_nodes->count(v_id) > 0) {
				//continue;
				root_number++;
				access_number += access_nodes[i].size(); 
			}
			outfile << "vertex: " << v_id << endl;
			outfile << "size: " << access_nodes[i].size() << endl;

			unordered_map<vertex_id, full_access_info>& v_map = access_nodes[i];
			unordered_map<vertex_id, full_access_info>::const_iterator an_iter = v_map.begin();
			unordered_map<vertex_id, full_access_info>::const_iterator an_iter_end = v_map.end();

			for(an_iter; an_iter != an_iter_end; ++an_iter) {
				vertex_id access_id = an_iter->first;
				edge_weight access_dist = an_iter->second.dist;
				vertex_id access_next = an_iter->second.next;
				outfile << access_id << " " << access_dist << " " << access_next << endl;  
			}
		}

		outfile.close();
		cout << "DONE!!" << endl;
		
		cout << "roots: " << root_number << endl << "access nodes:" << access_number << endl;
		cout << "mean value: " << access_number / root_number << endl;
	}	

}

#endif