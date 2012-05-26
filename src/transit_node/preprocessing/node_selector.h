#ifndef NODE_SELECTOR_H
#define NODE_SELECTOR_H

#include "../../tnr_utils/tnr_utils.h"


namespace tnr{
	class node_selector {
	public:		// nested class if need
	public:	//typedefs

	private:	//variable
		graph_t const &graph;
		path_map *visited_tree; //надо добавить константу и может шаред птр
		coordinate_check in_inner;
		coordinate_check in_outer;
		vertex_set selected_nodes;
		deque<vertex_id> priority_queue;
		vertex_set visited;
		int help_index;
		
	public:	//condtructors
		node_selector(graph_t const &graph): graph(graph), visited_tree(0), in_inner(0), in_outer(0) {}
		node_selector(graph_t const &graph, coordinate_check in_inner, coordinate_check in_outer):
			graph(graph), 
			visited_tree(0),
			in_inner(in_inner),
			in_outer(in_outer) {}
	private: //internal methods
		void inner_bfs(vertex_id root) {
			priority_queue.push_back(root);
			visited.insert(root);
			vertex_id v_id;

			while(priority_queue.size() != 0) {
				v_id = priority_queue[0];
				priority_queue.pop_front();

				vertex_t const *v = &(graph.get_vertex(v_id));
				coord_t const *coord;
		
				vertex_t::adj_iterator iter = v->out_begin();
				vertex_t::adj_iterator end = v->out_end();

				for(iter; iter != end; ++iter) {
					if(is_child(v_id, iter->v)){
						if(visited.count(iter->v) == 0) {
							coord = &(graph.get_vertex(iter->v).data.c);
							if(in_inner(*coord)) { // если сосед в о внутреннем квадрате
								visited.insert(iter->v);
								priority_queue.push_back(iter->v);
							} else if(in_outer(*coord)) { //если сосед только во внешнем квадрате
								if(outer_dfs(iter->v) == true) {
									selected_nodes.insert(std::min(v_id, iter->v)); 	
								}
							}else { // если сосед лежит за внешним квадратом
								selected_nodes.insert(v_id);
							}
						}//if(visited.count(iter->v) == 0)
					}//if(is_child(v_id, iter->v))
				}//for(iter; iter != end; ++iter)
			}

			
			
		}
		void inner_dfs(vertex_id v_id) {
			char c;
			if(visited.count(v_id) != 0) {
				std::cout << v_id << "  ujze byll!!!!!" << std::endl;
				std::cout << "parent is: " <<*((*visited_tree)[v_id].parent) << std::endl;
				std::cin>> c;
			} else {
				visited.insert(v_id);
			}

			vertex_t const *v = &(graph.get_vertex(v_id));
			coord_t const *coord; //координаты чайлдов
		
			vertex_t::adj_iterator iter = v->out_begin();
			vertex_t::adj_iterator end = v->out_end();
			
			for(iter; iter!= end; ++iter){
				if(is_child(v_id, iter->v)) {
					coord = &(graph.get_vertex(iter->v).data.c);
					if(in_inner(*coord)) {
						inner_dfs(iter->v);
					} else if(in_outer(*coord)) {
						if(outer_dfs(iter->v)) {
							//if(selected_nodes.count(iter->v) == 0) {
							//	std::cout<<"insert  " << ++help_index <<std::endl;
							//}
							selected_nodes.insert(std::min(v_id, iter->v)); 	
							//std::cout<<"true!!! return" << std::endl;
							//std::cin>> c;
							//никакого брэйка. может оказаться так что и v_id и iter->v access node'ы	
						}
					} else {
						selected_nodes.insert(v_id);
						break; // тк на пути ко всем его чайлдам уже будет лежать access_node
					}
				}
			} //for(iter; iter!= end; ++iter)
		} //void inner_dfs(vertex_id v_id)

		bool outer_dfs(vertex_id v_id) {
			/*if(v_id % 3 == 0)
				return true;
			else 
				return false;
			char c;
			
			std::cout<<"FANTASTIC!!" << std::endl;
			std::cin>>c;
			*/
			vertex_t const *v = &(graph.get_vertex(v_id));
			coord_t const *coord; //координаты чайлдов
			bool result = false;

			vertex_t::adj_iterator iter = v->out_begin();
			vertex_t::adj_iterator end = v->out_end();

			for(iter; iter!= end; ++iter) {
				if(is_child(v_id, iter->v)) {
					coord = &(graph.get_vertex(iter->v).data.c);
					if(in_outer(*coord)) {
						result = outer_dfs(iter->v); 
					} else {
						result = true;
					}

					if(result == true)
						return result;
				}
			}
			return false;
		}
		
		inline bool is_child(vertex_id parent, vertex_id may_be_child) {
			//std::cout << parent << " ? " << may_be_child;
			
			if(visited_tree->count(may_be_child) != 0) {
				path_vertex &child = (*visited_tree)[may_be_child];
				//std::cout << "  Child's parent: " << (*child.parent) << std::endl;
				if(child.parent.is_initialized()) {
					if((*child.parent) == parent) {
						//std::cout << " true!!" << std::endl;
						return true; 
					}
				} else {
					//std::cout << "not initialized parent!!" << std::endl;
				}
			}
			//std::cout << " false!!" << std::endl;
			return false;
		}

	public: //methods
		vertex_set const & select(path_map* visited_tree, vertex_id root) {
			selected_nodes.clear();
			if(in_inner == 0 || in_outer == 0) {
				std::cerr <<"you try to start node_selector without checkers" << std::endl;
				return selected_nodes;
			}

			this->visited_tree = visited_tree;
			selected_nodes.clear();
			visited.clear();
			priority_queue.clear();
			//help_index = 0;
			
			//inner_dfs(root);
			inner_bfs(root);
			return selected_nodes;
		}

	public:	//setters and getters
		void set_coordinate_checkers(coordinate_check &in_inner, coordinate_check &in_outer) {
			this->set_inner_checker(in_inner);
			this->set_outer_checker(in_outer);
		}

		void set_inner_checker(coordinate_check &in_inner) {
			this->in_inner = in_inner;
		}

		void set_outer_checker(coordinate_check &in_outer) {
			this->in_outer = in_outer;
		}
	};
}

#endif


