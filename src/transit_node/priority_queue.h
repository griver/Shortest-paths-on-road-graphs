#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H


#include "../shared/common_algorithms/path.h"
#include "../shared/new_vis_graph.h"
#include "algorithm_utils.h"


namespace my_algorithm{

	struct queue_vertex{
		edge_weight dist;
		vertex_id id;
	};

	bool operator<(queue_vertex const & a, queue_vertex const & b) {
		return a.dist < b.dist;
	}


	class my_priority_queue {
	private:
		size_t allocated;
		size_t size;
		std::vector<int> heap_indexes;
		std::vector<queue_vertex> heap;

	public:
		my_priority_queue (): size(0), allocated(0) {};
		void insert(my_graph::edge_weight dist, my_graph::vertex_id id) {
			heap_indexes[id] = size;
			heap[size].id = id;
			++size;
			decrease_key(dist, id);
		}
		void decrease_key(my_graph::edge_weight dist, my_graph::vertex_id id) {
			int index = heap_indexes[id];
 
			heap[index].dist = dist;
			while(index > 0 && heap[index] < heap[get_parent_id(index)]) {
				elem_swap(index, get_parent_id(index));
				index = get_parent_id(index);
			}
		}
		void reserve(size_t number) {
			allocated = number;
			heap.resize(allocated);
			heap_indexes.assign(allocated, -1);
		}

		inline queue_vertex top() const {
			return heap[0];
		}

		queue_vertex pop() {
			if(is_empty()) return queue_vertex();
			queue_vertex heap_top = heap[0];
			elem_swap(0, size - 1);
			size = size - 1;
			if(is_empty() == false)
				heapify(0);

			heap_indexes[heap_top.id] = -1;
			return heap_top;
		}

		bool is_empty() const {
			if(size == 0) return true;
			return false;
		}


	public:
		size_t get_parent_id(size_t index) {
			return index / 2;
		}
		size_t get_left_Iid(size_t index) {
			return index * 2;
		}
		size_t get_right_Id(size_t index) {
			return index * 2 + 1;
		} 

		void heapify(size_t index) {
			size_t left = get_left_Iid(index);
			size_t right = get_right_Id(index);
			size_t minimum = index;

			if(left < size && heap[left] < heap[minimum])
				minimum = left;
			if(right < size && heap[right] < heap[minimum]) 
				minimum = right;
			if(minimum != index) { 
				elem_swap(index, minimum);
				heapify(minimum);
			}
		}

		void elem_swap(size_t a, size_t b) {
			if(a == b) return;
			std::swap(heap_indexes[heap[a].id], heap_indexes[heap[b].id]);
			std::swap(heap[a], heap[b]);
		}

		int get_size() const {
			return this->size;
		}
		inline bool is_in_Queue(int id) {
			if(id < heap_indexes.size() && id >= 0) 
				if(heap_indexes[id] != -1)
					return true;
			return false;
		}
	};

}

#endif