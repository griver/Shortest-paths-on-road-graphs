#ifndef FULL_TNR_PATH_FINDER_H
#define FULL_TNR_PATH_FINDER_H

#include "../tnr_utils/tnr_utils.h"
#include "../tnr_utils/distance_table.h"

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
		
			if(start_access == -1 || end_access == -1) {
				cout << "path does not exist" << endl; 
				return;
			}
			//cout << "start access node: " << start_access << endl;
			//cout << "end access node: " << end_access << endl;

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

			this->build_path_from_access(end, end_access, resulting);

			//cout <<"resulting dist: " << resulting << endl;
			//cout <<"expected dist: " << expected << endl;

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
			size_t next_ind = -1;
			vertex_t::adj_iterator next_iter;
			while(current != access) {
				next_ind = ((*access_nodes)[current][access]).next;
				next_iter  = (graph.get_vertex(current).out_begin() + next_ind);

				next_edge = next_iter->e;
				next = next_iter->v;

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
			size_t next_ind = -1;
			vertex_t::adj_iterator next_iter;

			while(current != access) {
				next_ind = ((*access_nodes)[current][access]).next;
				access_dist = ((*access_nodes)[current][access]).dist;
				next_iter  = (graph.get_vertex(current).out_begin() + next_ind);

				next_edge = next_iter->e;
				next = next_iter->v;

				(*path)[current] = path_vertex(current, (common_dist + access_dist) , next_edge, next);
				current = next;
			}
			// возможно лучше прибавлять по  ребрам
			common_dist += ((*access_nodes)[base][access]).dist;
		}

		edge_weight find_dist(vertex_id start, vertex_id end, vertex_id &start_access, vertex_id &end_access) {
			unordered_map<vertex_id, access_info>::const_iterator start_iter = (*access_nodes)[start].begin();
			unordered_map<vertex_id, access_info>::const_iterator start_end = (*access_nodes)[start].end();
			unordered_map<vertex_id, access_info>::const_iterator end_iter = (*access_nodes)[end].begin();
			unordered_map<vertex_id, access_info>::const_iterator end_end = (*access_nodes)[end].end();
			float min_dist = FLT_MAX;
			float sum_dist;

			unordered_map<vertex_id, access_info>::const_iterator end_start = (*access_nodes)[end].begin();

			for(start_iter; start_iter != start_end; ++start_iter) {
				end_iter = end_start;
				for(end_iter; end_iter != end_end; ++end_iter) {
					
					sum_dist = start_iter->second.dist 
							+ end_iter->second.dist 
							+ table->get_dist(start_iter->first, end_iter->first);

					if(min_dist > sum_dist) {
						min_dist = sum_dist;
						start_access = start_iter->first;
						end_access = end_iter->first;
					}
				}
			}
			return min_dist;
		}

	};
	
}

#endif