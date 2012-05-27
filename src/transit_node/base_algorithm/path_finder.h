#ifndef PATH_FINDER_H
#define PATH_FINDER_H

#include "../shared/common.h"
#include "../shared/common_algorithms/path.h"
#include "../shared/new_vis_graph.h"
#include <functional>
#include <map>
#include <set>
#include "priority_queue.h"
#include "filter/queue_filter.h"
#include "filter/coordinate_filter.h"

namespace base_algorithm {

	class path_finder{
	public:
										
		typedef boost::function<my_graph::edge_weight (edge_t const &)> weight_function;	//Function gets Edge end returns edge lenght. 
		typedef boost::function<my_graph::edge_weight (vertex_t const &, vertex_t const &)> estimate_function; //Estimate function for A* algorithm

	private:
		graph_t const  *graph_;					// собственно наш  граФ
		path_map* visited_;						// наши посещенные вершины 
		path_map* other_visited_;				// вершины посешенные встречным поиском. 
		my_priority_queue priority_queue_;			// очередь с прироитетом
		weight_function get_weight_;				// функция для получения весов ребер
		estimate_function get_estimate_;			// функция для эвристической оценки алгоритма
	private:
		boost::function<my_graph::edge_weight(vertex_t const &)> dist_to_target_;
		vertex_id source_;
		vertex_id target_;
		bool is_initialized_;
		bool is_backward_;
		std::set<edge_id>* border_set_;  
	private:
		shared_ptr<queue_filter> filter_ptr_;


	public:
		//visited - tree of vertices viewed by our dijkstra
		//visited type - unordered_map<vertex_id, path_vertex>
		//конструктор с функцией
		path_finder(graph_t const *graph,
			weight_function const &get_weight,
			estimate_function const &get_estimate,
			my_graph::path_map* visited): graph_(graph),get_weight_(get_weight), visited_(visited), get_estimate_(get_estimate){
			filter_ptr_ = shared_ptr<queue_filter>(new queue_filter("Deafault Filter"));
			this->init(graph->v_count());
		}

		path_finder(graph_t const *graph,
			weight_function const &get_weight,
			my_graph::path_map* visited): graph_(graph),get_weight_(get_weight), visited_(visited), get_estimate_(default_estimate){
			filter_ptr_ = shared_ptr<queue_filter>(new queue_filter("Deafault Filter"));
			this->init(graph->v_count());
		}

		void init(size_t vertex_number) {	
			//std::cout <<"init" << std::endl;
			this->priority_queue_.reserve(vertex_number + 1);

			visited_->clear();
			filter_ptr_->clear();
			
			border_set_ = NULL;
			other_visited_ = NULL;
			this->is_initialized_ = false;
			//std::cout <<"init end" << std::endl;
		}

	public:
		void init_search(my_graph::vertex_id start_vertex, 
			my_graph::vertex_id end_vertex, 
			bool is_backward = false, 
			path_map* other_visited = 0, 
			std::set<edge_id>* border_set = 0) { //init_search

			this->init(graph_->v_count());
			this->source_ = start_vertex;
			this->target_ = end_vertex;
			this->is_backward_ = is_backward;
			this->other_visited_ = other_visited;
			this->border_set_ = border_set;

			dist_to_target_ = (boost::bind(get_estimate_, _1, graph_->get_vertex(end_vertex)));
			this->priority_queue_.insert(0.0, start_vertex);
			//std::cout <<"on push start" << std::endl;
			on_push(start_vertex, start_vertex);	// третья проверка фильтра. всего три.
			//std::cout <<"end on push start" << std::endl;
			(*visited_)[start_vertex] = path_vertex(start_vertex, 0.0);
			
			this->is_initialized_ = true;
		}

		// итерации алгоритмам
		inline void iterate() {
			int zzz = 0;
			queue_vertex heap_vertex;						//priority_queue_ top 
			vertex_t const *v = 0;						//curent vertex 
			vertex_t const *u = 0;						//conjugate vertex for v
			edge_weight weight_v_u = 0.0f;				//lenght of directed edge (v,u)
			edge_weight estimate = 0.0f;
			my_graph::vertex_id v_id = -1;				//v->get_id();
			my_graph::vertex_id u_id = -1;				//u->get_id();

			heap_vertex = priority_queue_.pop();				//deleteMin
			if(on_pop(heap_vertex.id) != true) {		// Здесь как раз проверка фильтра!
				std::cout << "WHAT THE FUCK!!!" << std::endl;
				return;
			}

			v = &(graph_->get_vertex(heap_vertex.id));
			v_id = heap_vertex.id;

			// обратный поиск будет просматривать по входящим вершинам.
			vertex_t::adj_iterator iter;
			vertex_t::adj_iterator end;
			if(is_backward_) {
				iter = v->in_begin();
				end = v->in_end();
			} else {
				iter = v->out_begin();
				end = v->out_end();
			}
			/*static int val = 0;
			cout <<"-----------------------------" << endl; 
			cout << "iteration: " << ++val << endl;
			cout << "vertex: " << v_id << " has: " << end - iter << endl;*/

			for(iter; iter != end; ++iter) {
				u_id = (*iter).v;

				if(is_backward_)
					u = &(graph_->get_vertex(u_id));
				else
					u = &(graph_->get_vertex(u_id));
						
				weight_v_u = get_weight_(/**/graph_->get_edge((*iter).e)/**/);
				estimate = dist_to_target_(graph_->get_vertex(u_id));
				//cout << "    neighbor: " << u_id << " edge v to u: " << weight_v_u << endl;	
				if(other_visited_ != 0)
					if(border_set_ != 0)
						if(other_visited_->count(u_id) != 0)
							border_set_->insert((*iter).e);

				if(visited_->count(u_id) == 0) {
					(*visited_)[u_id] = my_graph::path_vertex(u_id, 
														(*visited_)[v_id].d + weight_v_u,
														(*iter).e, v_id);
					if(on_push(v_id, u_id) == true ) 
						priority_queue_.insert((*visited_)[u_id].d + estimate, u_id);
					

				}else if((*visited_)[u_id].d > ((*visited_)[v_id].d + weight_v_u) ) {
					(*visited_)[u_id].d  = (*visited_)[v_id].d + weight_v_u;
					(*visited_)[u_id].inc = (*iter).e;
					(*visited_)[u_id].parent = v_id;

					if(priority_queue_.is_in_Queue(u_id)) {
						if(on_decrease(v_id, u_id) == true) 
							priority_queue_.decrease_key((*visited_)[u_id].d + estimate, u_id);
						else
							std::cout << "WHAT THE FUCK!!!" << std::endl;			
					} else {
						if(on_push(v_id, u_id) == true)
							priority_queue_.insert((*visited_)[u_id].d + estimate, u_id);
						else 
							std::cout << "WHAT THE FUCK!!!" << std::endl;
					} //if(priority_queue_.is_in_Queue(u_id))
				}
										   
			}//for(Vertex::ve_const_iterator iter = v->out_begin(); iter != v->out_end(); ++iter) {
		}

	public:
		//Следующая вершина которая будет вытащена из очереди
		inline vertex_id get_next() const {
			if(!priority_queue_.is_empty())
				return priority_queue_.top().id;
			else
				return -1;
		}


		//достиг ли поиск цели
		inline bool is_find() const {
			if(!priority_queue_.is_empty())
				if(priority_queue_.top().id == target_)
					return true;
			return false;
		}
		// устанавливает фильтер на добавление и узаление элементов из очереди.
		void set_priority_queue_filter( shared_ptr<queue_filter> &ptr) {
			filter_ptr_ = ptr;
		}

		//закончен ли поиск
		inline bool is_stopped() const {
			return priority_queue_.is_empty();
		}

		//пересекся ли со встречным поиском
		inline bool is_intersect() const {
			//std::cout << "In Function is_intersect()" << std::endl;
			if(other_visited_ != 0)
				if(other_visited_->count(priority_queue_.top().id) != 0)
					return true;
			return false;	

		}
		//коректно ли инициализирован поиск
		inline bool is_initialized() const {
			return is_initialized_;
		}

	public:
		inline bool on_pop(vertex_id id) const {	
			return filter_ptr_->on_pop(graph_->get_vertex(id));
		}

		inline bool on_push(vertex_id parent, vertex_id vertex) const { 
			return filter_ptr_->on_push(graph_->get_vertex(parent),graph_->get_vertex(vertex));
		}
		
		inline bool on_decrease(vertex_id parent, vertex_id vertex) const {
			return filter_ptr_->on_decrease(graph_->get_vertex(parent), graph_->get_vertex(vertex));
		}
	};

}

#endif