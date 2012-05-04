#ifndef COORDINATE_FILTER_H
#define COORDINATE_FILTER_H
#include "queue_filter.h"

namespace my_algorithm {

	class coordinate_filter : public queue_filter {
	public:
		typedef boost::function<bool (coord_t const &)> check_function;

	private:
		check_function check;
		int counter;
	public: 
		coordinate_filter(string const &name, check_function check) : queue_filter(name), check(check), counter(0) {};

		virtual bool on_pop(vertex_t const &vertex) {
			if(check(vertex.data.c)) --counter;
			return queue_filter::on_pop(vertex);
		}

		virtual bool on_push(vertex_t const &vertex) {
			if(check(vertex.data.c)) ++counter;
			return queue_filter::on_push(vertex);
		}
	
		void set_check_function(check_function &check) {
			this->check = check;
		}

		void clear() {
			counter = 0;
		}

		int get_counter() const {
			return counter;
		}
	};
}

#endif