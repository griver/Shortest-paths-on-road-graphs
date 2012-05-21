#ifndef TRANSIT_NODE_FILTER_H
#define TRANSIT_NODE_FILTER_H
#include "queue_filter.h"
#include "tnr_utils.h"

namespace tnr {
	enum reached_state{
		AFTER_TNR = 7,
		BEFORE_TNR = 8
	};

	class transit_node_filter : public my_algorithm::queue_filter {
	private:	
		shared_ptr<vertex_set> transit_nodes;
		unordered_map<vertex_id, reached_state> when_reached; 
		vertex_id start_vertex;
		int nodes_before;

	public: 
		transit_node_filter(string const &name, shared_ptr<vertex_set>& tn, vertex_id start) : queue_filter(name), transit_nodes(tn), start_vertex(start) {
			nodes_before = 0;
		};

		virtual bool on_pop(vertex_t const &vertex) {
			if(check(vertex.id))
				if(when_reached[vertex.id] == BEFORE_TNR) // Если на пути к этой вершине не встречался tn
					--nodes_before; 

			return queue_filter::on_pop(vertex);
		}

		virtual bool on_push(vertex_t const &parent, vertex_t const &vertex) {
			//std::cout << vertex.id << "  start:" << start_vertex << std::endl;
			if(vertex.id == start_vertex) {			// добавили стартовую вершину
				++nodes_before;
				when_reached[vertex.id] = BEFORE_TNR; //!
				return queue_filter::on_push(parent, vertex);
			}

			if(check(parent.id)) {
				if(when_reached[parent.id] == BEFORE_TNR)
					if(parent.id == start_vertex || transit_nodes->count(parent.id) == 0) { 
						++nodes_before;
						when_reached[vertex.id] = BEFORE_TNR;  //!
						return queue_filter::on_push(parent, vertex);
					}
					when_reached[vertex.id] = AFTER_TNR;  //!
				return queue_filter::on_push(parent, vertex);
			}
			return queue_filter::on_push(parent, vertex);
		}
		
		virtual bool on_decrease(vertex_t const &parent, vertex_t const &vertex) {
			if(!check(parent.id)) queue_filter::on_decrease(parent, vertex);
			
			reached_state parent_state;
			if(when_reached[parent.id] == AFTER_TNR) {
				parent_state = AFTER_TNR;
			} else if(transit_nodes->count(parent.id) > 0 && parent.id != start_vertex) {
				parent_state = AFTER_TNR;
			} else {
				parent_state = BEFORE_TNR;
			}

			if(parent_state != when_reached[vertex.id]) {
				if(parent_state == BEFORE_TNR)
					++nodes_before;
				else
					--nodes_before;
				when_reached[vertex.id] = parent_state;
			}
				
			return queue_filter::on_decrease(parent, vertex);
		}

		virtual void clear() {
			queue_filter::clear();
			this->when_reached.clear();
			this->nodes_before = 0;
		}

		void set_start(vertex_id start) {
			start_vertex = start;
		}

		void set_transit_set(shared_ptr<vertex_set>& transit_nodes) {
			this->transit_nodes = transit_nodes;
		}
		
		int get_node_before() const {
			return nodes_before;
		}
		
		unordered_map<vertex_id, reached_state> const & get_when_reached_map() const {
			return when_reached;
		}

	private:
		inline bool check(vertex_id vertex) {
			if(when_reached.count(vertex) > 0) {
				return true;
			} else {
				std::cout << "NONSENSE!!" << std::endl;
				return false;
			}
		}
	
	};
}

#endif