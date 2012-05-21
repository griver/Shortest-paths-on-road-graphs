#ifndef FULL_DISTANCE_TABLE_PREPROCESSOR_H
#define FULL_DISTANCE_TABLE_PREPROCESSOR_H

#include "tnr_utils.h"
#include "distance_table.h"

namespace tnr {

	class full_distance_table_preprocessor {
	private: ///
		vis_graph const &graph;
		shared_ptr<full_access_map> access_nodes;
		shared_ptr<vertex_set> transit_nodes;
		shared_ptr<vis_graph> tn_graph;
		unordered_map<vertex_id, vertex_id> mapping;
		unordered_map<vertex_id, vertex_id> r_mapping;
		shared_ptr< dist_table> table;

	public: ///
		full_distance_table_preprocessor(vis_graph const &graph, shared_ptr<full_access_map>& access_nodes, shared_ptr<vertex_set>& transit_nodes)
			:graph(graph),
			access_nodes(access_nodes),
			transit_nodes(transit_nodes),
			table(new  dist_table(transit_nodes)),
			tn_graph(new vis_graph()){
				cout << "constructor()" << endl;
				this->create_transit_nonde_graph();
		}

	public:
		void calculate() {
			vertex_set::const_iterator tn_iter = (*transit_nodes).begin();
			vertex_set::const_iterator tn_end = (*transit_nodes).end();
			path_map visited;
			int i = 0;
			for(tn_iter; tn_iter != tn_end; ++tn_iter) { // главный цикл
				vertex_id tn_id = (*tn_iter);
				this->run_dijkstra_to_all(tn_id, visited);
				this->fill_table_to(tn_id, visited);
				cout << "visited vertices: " << visited.size() << "    " << ++i << endl;
			} //for(tn_iter; tn_iter ! =tn_end; ++tn_iter)

		}

		shared_ptr< dist_table>& get_table() {
			return table;
		}

		shared_ptr<vis_graph>& get_tn_graph() {
			return tn_graph;
		}

		void write_to(string const & filename) const {
			write_dist_table_to(*table, filename);
		}

	private:
		void create_transit_nonde_graph() {
			
			vertex_set::const_iterator tn_iter = transit_nodes->begin();
			for(tn_iter; tn_iter != transit_nodes->end(); ++tn_iter) {
				vertex_id t_id= (*tn_iter);
				vertex_id new_id = tn_graph->add_vertex(graph.get_vertex(t_id).data);
				mapping[t_id] = new_id;
				r_mapping[ new_id ] = t_id;
			}

			tn_iter = transit_nodes->begin();

			for(tn_iter; tn_iter != transit_nodes->end(); ++tn_iter) {
				vertex_id t_id= (*tn_iter);
				
				unordered_map<vertex_id, full_access_info>::const_iterator an_iter = (*access_nodes)[t_id].begin();
				unordered_map<vertex_id, full_access_info>::const_iterator an_end_iter = (*access_nodes)[t_id].end();

				for(an_iter; an_iter != an_end_iter; ++an_iter) {
					vertex_id an_id = an_iter->first;
					edge_weight an_dist = an_iter->second.dist;
					if(t_id < an_id)
						tn_graph->add_edge(mapping[t_id], mapping[an_id], vis_edge_data(an_dist));
				}
			}
			
		}


		void run_dijkstra_to_all(vertex_id start_vertex, path_map& visited) {
			path_finder finder(tn_graph.get(), get_weight, &visited);
			
			vertex_id tn_id  = mapping[start_vertex];
			finder.init_search(tn_id, tn_id);
			while(!finder.is_stopped()) {
				if(finder.is_intersect()) break;
				finder.iterate();
			}
		}


		void fill_table_to(vertex_id start, path_map& visited) {
			vertex_set::const_iterator tn_iter = (*transit_nodes).begin();
			vertex_set::const_iterator tn_end = (*transit_nodes).end();
			for(tn_iter; tn_iter != tn_end; ++tn_iter) {
				
				vertex_id id = (*tn_iter);
				vertex_id new_id = mapping.at(id);
				if(visited.count(new_id) > 0) {
					if(id != start) {
						vertex_id next = r_mapping.at(*(visited[new_id].parent));
						edge_weight dist = visited[new_id].d;
						table->set_cell(id, start, next, dist);		
					} else {
						table->set_cell(start, start, start, 0.0f);		
					}
				} else {
					cout << "strange mapping!!!!!!!!" << endl;
				}
			
			}//for(tn_iter; tn_iter != tn_end; ++tn_iter)
		}
	};
}

#endif