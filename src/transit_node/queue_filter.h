#ifndef QUEUE_FILTER_H
#define QUEUE_FILTER_H
#include "algorithm_utils.h"

namespace my_algorithm {	
	class queue_filter {
	private:
		shared_ptr<queue_filter> next;
		string name;	
	public:
		explicit queue_filter(string const &name): name(name), next() {
		}
	public:
		virtual bool on_pop(vertex_t const &vertex) {
			if(has_next())
				return next->on_pop(vertex);
			return true;
		}

		virtual bool on_push(vertex_t const &parent, vertex_t const &vertex) {
			if(has_next())
				return next->on_push(parent, vertex);
			return true;
		}
		virtual bool on_decrease(vertex_t const &parent, vertex_t const &vertex) {
			if(has_next())
				return next->on_decrease(parent, vertex);
			return true;
		}

		virtual void clear() {
			if(has_next())
				return next->clear();
		}

		bool has_next() const {
			//cout << "in has_next() of" << name <<" next = " << next.get() << endl;  
			return (next.get() != 0);
		}
		
		void add_filter(shared_ptr<queue_filter> filter) {
			if(has_next()) {
				next->add_filter(filter);
			} else {
				next = filter;
			}
		}
		
		shared_ptr<queue_filter>& get_next() {
			return next;
		}
		
		string const & get_name() const {
			return name;
		}
		
		shared_ptr<queue_filter> remove_filter(string name) {
			shared_ptr<queue_filter> tmp;
			if(has_next()) { // Если есть следующий
				if(next->get_name() == name) {	// если его нужно удалить
					tmp = next;					//тот указатель что мы вернем
					next.reset();			
					if(tmp->has_next()) {
						next = tmp->get_next();
						tmp->next.reset();
					}
				}
			}
			return tmp;
		}	

		virtual ~queue_filter() {
			//cout<<" destructor for "<<name << endl;
			if(has_next())
				next.reset();
		}
	};
}


#endif