#ifndef COORDINATE_FILTER_H
#define COORDINATE_FILTER_H
#include "queue_filter.h"


namespace my_algorithm {

	class coordinate_filter : public queue_filter {
	private:
		coordinate_check check;
		int counter;
	public: 
		coordinate_filter(string const &name, coordinate_check check) : queue_filter(name), check(check), counter(0) {};

		virtual bool on_pop(vertex_t const &vertex) {
			if(check(vertex.data.c)) --counter;
			return queue_filter::on_pop(vertex);
		}

		virtual bool on_push(vertex_t const &parent, vertex_t const &vertex) {
			if(check(vertex.data.c)) ++counter;
			return queue_filter::on_push(parent, vertex);
		}
	
		virtual bool on_decrease(vertex_t const &parent, vertex_t const &vertex) {
			return queue_filter::on_decrease(parent, vertex);
		}

		void set_coordinate_checker(coordinate_check &check) {
			this->check = check;
		}

		virtual void clear() {
			queue_filter::clear();
			counter = 0;
		}

		int get_counter() const {
			return counter;
		}
	};
}

#endif