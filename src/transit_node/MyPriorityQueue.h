#ifndef MY_PRIOTITY_QUEUE_H
#define MY_PRIOTITY_QUEUE_H

#include "../shared/common_algorithms/path.h"
#include "../shared/graph.h"


struct QueueVertex {
	my_graph::edge_weight dist;
	my_graph::edge_id id;
};

bool operator<(QueueVertex const & a, QueueVertex const & b) {
	return a.dist < b.dist;
}


class MyPriorityQueue {
private:
	size_t allocated;
	size_t size;
	std::vector<int> heapIndexes;
	std::vector<QueueVertex> heap;

public:
	MyPriorityQueue (): size(0), allocated(0) {};
	void insert(my_graph::edge_weight dist, my_graph::vertex_id id) {
		heapIndexes[id] = size;
		heap[size].id = id;
		++size;
		decreaseKey(dist, id);
	}
	void decreaseKey(my_graph::edge_weight dist, my_graph::vertex_id id) {
		int index = heapIndexes[id];
		float oldDist = heap[index].dist; 
		heap[index].dist = dist;
		while(index > 0 && heap[index] < heap[getParentId(index)]) {
			elemSwap(index, getParentId(index));
			index = getParentId(index);
		}
	}
	void reserve(size_t number) {
		allocated = number;
		heap.resize(allocated);
		heapIndexes.assign(allocated, -1);
	}

	QueueVertex top() {
		return heap[0];
	}

	QueueVertex pop() {
		if(isEmpty()) return QueueVertex();
		QueueVertex heapTop = heap[0];
		elemSwap(0, size - 1);
		size = size - 1;
		if(isEmpty() == false)
			heapify(0);

		heapIndexes[heapTop.id] = -1;
		return heapTop;
	}

	bool isEmpty() {
		if(size == 0) return true;
		return false;
	}

public:
	size_t getParentId(size_t index) {
		return index / 2;
	}
	size_t getLeftId(size_t index) {
		return index * 2;
	}
	size_t getRightId(size_t index) {
		return index * 2 + 1;
	} 

	void heapify(size_t index) {
		size_t left = getLeftId(index);
		size_t right = getRightId(index);
		size_t minimum = index;

		if(left < size && heap[left] < heap[minimum])
			minimum = left;
		if(right < size && heap[right] < heap[minimum]) 
			minimum = right;
		if(minimum != index) { 
			elemSwap(index, minimum);
			heapify(minimum);
		}
	}

	void elemSwap(size_t a, size_t b) {
		if(a == b) return;
		std::swap(heapIndexes[heap[a].id], heapIndexes[heap[b].id]);
		std::swap(heap[a], heap[b]);
	}

	int getSize() const {
		return this->size;
	}

	inline bool isInQueue(int id) {
		if(id < heapIndexes.size() && id >= 0) 
			if(heapIndexes[id] != -1)
				return true;
		return false;
	}
};

#endif