#include "../../../tnr_utils/tnr_utils.h"
#include "../../../tnr_utils/grid_info.h"
#include "../tnr_algorithms.h"

namespace tnr{
	class shortcut_preprocessor {
	private:
		vector<vertex_id> mapping;
		vector<vertex_id> r_mapping;
		vector<int> excluded;

		vector<int> visited;
		vector<double> dist;

		graph_t &old_graph;
		shared_ptr<graph_t> new_graph;
		grid_info & info;
	public:
		shortcut_preprocessor(graph_t &old_graph, grid_info & info): old_graph(old_graph), new_graph(new graph_t()), info(info) {
			mapping.resize(old_graph.v_count());
			r_mapping.resize(old_graph.v_count());
			excluded.resize(old_graph.v_count());
			excluded.assign(excluded.size(), 0);

			visited.resize(old_graph.v_count());
			visited.assign(visited.size(), 0);
			dist.resize(old_graph.v_count());
		}
	public:
		shared_ptr<graph_t>& calculate_shourtcuts() {
			cout << "calculate_shourtcuts()" << endl;
			size_t old_size = old_graph.v_count();
			
			for(size_t i = 0; i < old_size; ++i) {
				if(i % 10000 == 0)
					cout << i << endl;
				if(excluded[i] == 0) {
					shortcut_bfs(i, (i + 1));
				}
			}

			if(old_graph.v_count() != old_size)
				cout << "size changed!!" << endl;

			for(size_t i = 0;  i < old_graph.v_count(); ++i) {
				if(excluded[i] != 0)
					continue;
				vertex_id old_id = i;
				vertex_t& old_vertex = old_graph.get_vertex(old_id);
				vertex_id new_id = new_graph->add_vertex(old_vertex.data);

				r_mapping[new_id] = old_id;
				mapping[old_id] = new_id;

				for(vertex_t::adj_iterator adj = old_vertex.out_begin(); adj < old_vertex.out_end(); ++adj) {
					if(excluded[adj->v] != 0)
						continue;
					if(adj->v < old_id) {
						new_graph->add_edge(new_id, mapping[adj->v], old_graph.get_edge(adj->e).data);
					}
				} // цикл по ребрам	
			}	//цикл по вершинам

			return new_graph;
		}


		shared_ptr<vertex_set> calculate_transit_nodes(path_map *visited_tree, path_map *shortest_path) {
			info.calculate_border_vertices(*new_graph, shortest_path);

			vertex_set new_transit_nodes;

			border_map & grid_borders = info.get_border_map();

			border_map::iterator iter = grid_borders.begin();
			border_map::iterator end = grid_borders.end();

			path_finder finder(new_graph.get(), get_weight, visited_tree);

			node_selector selector(*new_graph);
			for(iter; iter != end; ++iter) {
//				if(iter->first.x == 90 && iter->first.y == 47)
				calculate_cell_access_nodes(iter->first, iter->second, finder, selector, new_transit_nodes, info, visited_tree);
			}
			
			shared_ptr<vertex_set> transit_nodes(new vertex_set());
			for(vertex_set::iterator i = new_transit_nodes.begin(); i != new_transit_nodes.end(); ++i){
				vertex_id new_id = *i;
				(*transit_nodes).insert(r_mapping[new_id]);
			}

			return transit_nodes;
		}


	private:
		void shortcut_bfs(vertex_id start , int true_number) {
			deque<vertex_id> queue;
			deque<vertex_id> bfs_end;
			
			queue.push_back(start);
			visited[start] = true_number;
			dist[start] = 0.0f;
			coord_t start_coord = old_graph.get_vertex(start).data.c;

			while(!queue.empty()) {
				vertex_id v_id = queue[0];
				queue.pop_front();
				vertex_t &v = old_graph.get_vertex(v_id);
				excluded[v_id] = true_number;

				for(vertex_t::adj_iterator adj = v.out_begin(); adj != v.out_end(); ++adj) {
					if(excluded[adj->v] != 0)
						continue;
					if(visited[adj->v] == true_number)
						continue;

					visited[adj->v] = true_number;
					dist[adj->v] = dist[v_id] + old_graph.get_edge(adj->e).data.len;
					//если сосед находится сликом далеко
					if(is_far_enough(start_coord, old_graph.get_vertex(adj->v).data.c)) {
						if(v_id != start)
							bfs_end.push_back(v_id);
						continue;
					}

					if(get_degree(adj->v, true_number) != 2) {
						if(v_id != start)
							bfs_end.push_back(adj->v);
						continue;
					}
					queue.push_back(adj->v);
				}
			}// while(!queue.empty()) {
			

			//обработать результаты
			excluded[start] = 0;
			for(int i = 0; i < bfs_end.size(); ++i) {
				vertex_id end = bfs_end[i];
				excluded[bfs_end[i]] = 0;
				old_graph.add_edge(start, end, vis_edge_data(dist[end])); 
			}
			//if(bfs_end.size() != get_degree(start))
			//	cout << "bfs has" << bfs_end.size() << " ends but start has " << get_degree(start) <<" degree" << endl;  
		}//shordcut_bfs(vertex_id start) {
		
		bool is_far_enough(coord_t const & a, coord_t const & b) {
			if(abs(info.get_x_cell(a.x) - info.get_x_cell(b.x)) > 3)
				return true;
			if(abs(info.get_y_cell(a.y) - info.get_y_cell(b.y)) > 3)
				return true;
			return false;
		}

		size_t get_degree(vertex_id id, int iteration) {
			size_t degree = 0;
			vertex_t::adj_iterator iter = (old_graph.get_vertex(id)).out_begin();
			vertex_t::adj_iterator end = (old_graph.get_vertex(id)).out_end();
			for(iter; iter != end; ++iter)
				if(excluded[iter->v] == 0 || excluded[iter->v] == iteration)
					++degree;
			return degree;
		}
	};


	void write_tansit_nodes_to_file(string filename, shared_ptr<vertex_set>& transit_nodes) {
		std::ofstream outfile (filename.c_str());
		for(vertex_set::const_iterator i = transit_nodes->begin(); i != transit_nodes->end(); ++i)
			outfile << (*i) << endl;
		outfile.close();
	}
}