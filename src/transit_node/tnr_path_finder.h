#ifndef TNR_PATH_FINDER_H
#define TNR_PATH_FINDER_H

#include "tnr_utils.h"
#include "distance_table.h"
#include "path_building_exception.h"
/*
(a(2883) != b(2880))
216
path from: 18603 to: 7276 : PROBLEM!!!!

(a(1910) != b(1908))
52
path from: 928 to: 29952 :     PROBLEM!!!!

//*/
namespace tnr {

	class tnr_path_finder {
	private:
		graph_t const & graph;
		shared_ptr<access_map> access_nodes;
		shared_ptr<dist_table> table;
		path_map* path;
		//vertex_id start_access;
		//vertex_id end_access;
	public:
		tnr_path_finder(graph_t const & graph, shared_ptr<access_map>& access_nodes, shared_ptr<dist_table>& table, path_map* path)
			:graph(graph), access_nodes(access_nodes), table(table), path(path) {}

		void search(vertex_id start, vertex_id end) {
			init_search();
			vertex_id start_access = -1; 
			vertex_id end_access = -1;
			edge_weight expected;
			
			expected = find_dist(start, end, start_access, end_access);
			//cout <<"expected dist: " << expected << endl;
			//return;
			//end_access = 7511;
			if(start_access == -1 || end_access == -1) {
				cout << "path does not exist" << endl; 
				return;
			}
			cout << "start access node: " << start_access << endl;
			cout << "end access node: " << end_access << endl;
			edge_weight resulting = 0.0f;
			vertex_id current = start_access;
			vertex_id next;

			this->build_path_to_access(start, start_access, resulting);
			//cout << "before while " << endl;
			while(current != end_access) {
				next = table->get_next(current, end_access);
				this->build_path_to_access(current, next, resulting);
				current = next;
			}
			//cout << "after while " << endl;
			if(resulting != (*path)[end_access].d) {
				cout <<"LAST ACCESS NODE INCORECT DIST" << endl;
				cout << (*path)[end_access].d << endl;
				cout <<  resulting << endl;
			}

			this->build_path_from_access(end, end_access, resulting);

			cout <<"resulting dist: " << resulting << endl;
			//cout <<"expected dist: " << expected << endl;

			if(resulting != (*path)[end].d) {
				cout <<"LAST ACCESS NODE INCORECT DIST" << endl;
				cout << (*path)[end_access].d << endl;
				cout <<  resulting << endl;
			}
		}

	private:
		void init_search() {
			path->clear();
		}
		inline edge_id get_next_edge(vertex_id v, vertex_id u) {
			vis_vertex::adj_iterator iter = graph.get_vertex(v).out_begin();
			vis_vertex::adj_iterator end = graph.get_vertex(v).out_end();
			for(iter; iter != end; ++iter) {
				if(iter->v == u)
					return iter->e;
			}

			throw path_building_exception("cannot find next edge in get_next_edge()");
			return -1;
		}


		void build_path_to_access(vertex_id base, vertex_id access, edge_weight& common_dist) {
			vertex_id current = base;
			edge_id next_edge = -1;
			vertex_id next = -1;

			while(current != access) {
				next = ((*access_nodes)[current][access]).next;
				next_edge = get_next_edge(current, next);	
				common_dist += graph.get_edge(next_edge).data.len;
				(*path)[next] = path_vertex(next, common_dist, next_edge, current);
				current = next;
			}
		}
		
		void build_path_from_access(vertex_id base, vertex_id access, edge_weight& common_dist) {
			vertex_id current = base;
			edge_id next_edge = -1;
			vertex_id next = -1;
			edge_weight access_dist = 0.0f;

			while(current != access) {
				next = ((*access_nodes)[current][access]).next;
				access_dist = ((*access_nodes)[current][access]).dist;
				next_edge = get_next_edge(current, next);

				(*path)[current] = path_vertex(current, (common_dist + access_dist) , next_edge, next);
				current = next;
			}

			common_dist += ((*access_nodes)[base][access]).dist;
		}

		edge_weight find_dist(vertex_id start, vertex_id end, vertex_id &start_access, vertex_id &end_access) {
			unordered_map<vertex_id, access_info>::const_iterator start_iter = (*access_nodes)[start].begin();
			unordered_map<vertex_id, access_info>::const_iterator start_end = (*access_nodes)[start].end();
			unordered_map<vertex_id, access_info>::const_iterator end_iter = (*access_nodes)[end].begin();
			unordered_map<vertex_id, access_info>::const_iterator end_end = (*access_nodes)[end].end();
			double min_dist = DBL_MAX;
			

			//cout << "start access nodes number: " <<(*access_nodes)[start].size() << endl;
			//cout << "end access nodes number: " <<(*access_nodes)[end].size() << endl;

			for(start_iter; start_iter != start_end; ++start_iter) {
				end_iter = (*access_nodes)[end].begin();
				for(end_iter; end_iter != end_end; ++end_iter) {
					
					double to_start = (double)start_iter->second.dist;
					double to_end = (double)end_iter->second.dist;
					double between = (double)table->get_dist(start_iter->first, end_iter->first);
					if(start_iter->first == 4519 && (end_iter->first == 7511 || end_iter->first == 251841)) {
						cout << start_iter->first << "  " << end_iter->first << endl;   
						cout << to_start << "+" << between << "+" << to_end 
							<< " = " << (to_start +  between + to_end) << endl;
					}
	

					if(min_dist > (double)(to_start +  between + to_end)) {
						min_dist = (to_start +  between + to_end);
						start_access = start_iter->first;
						end_access = end_iter->first;

				
					}
				}
			}
			return (float)min_dist;
		}

	};
	
}

#endif