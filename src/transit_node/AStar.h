#ifndef A_STAR_H
#define A_STAR_H

#include "../shared/common.h"
#include "../shared/common_algorithms/path.h"
#include "../shared/graph.h"
#include "../shared/coord.h"
#include <functional>
#include <map>
#include "PriorityQueue.h"
#include "VisGraphDistance.h"
#include "AStar.h"

namespace nir {


/*template<class V, class E> 
inline edge_weight defaultEstimate(vertex_base<V, E> const & source, vertex_base<V, E> const & target) {
	return 0.0f;
}*/


template <class VertexData, class EdgeData>
class AStar{
public:
	typedef my_graph::graph_base<VertexData, EdgeData>	Graph;			// Our graph parameterized by data types
	typedef my_graph::edge_base<VertexData, EdgeData>	Edge;
	typedef my_graph::vertex_base<VertexData, EdgeData>	Vertex;										
	typedef boost::function<my_graph::edge_weight (Edge const &)> WeightFunction;	//Function gets Edge end returns edge lenght. 
	typedef boost::function<my_graph::edge_weight (Vertex const &, Vertex const &)> EstimateFunction; //Estimate function for A* algorithm
	typedef my_graph::edge_weight EdgeWeight;

private:
	Graph const  *graph_;
	unordered_map<my_graph::vertex_id, my_graph::path_vertex> helpMap;
	my_graph::path_map* visited_;
	MyPriorityQueue priorityQueue_;
	WeightFunction getWeight_;
	EstimateFunction getEstimate_;
	

private:
	typedef boost::function<float (Vertex const &, Vertex const &)> EstimateFunction;

public:
	//visited - tree of vertices viewed by our dijkstra
	//visited type - unordered_map<vertex_id, path_vertex>
	//конструктор с функцией
	AStar(Graph const *graph,
		WeightFunction const &getWeight,
		EstimateFunction const &getEstimate,
		my_graph::path_map* visited): graph_(graph),getWeight_(getWeight), visited_(visited), getEstimate_(getEstimate){

		this->init(graph->v_count());
	}

		AStar(Graph const *graph,
		WeightFunction const &getWeight,
		my_graph::path_map* visited): graph_(graph),getWeight_(getWeight), visited_(visited), getEstimate_(defaultEstimate){

		this->init(graph->v_count());
	}

	void init(size_t vertexNumber) {
		this->priorityQueue_.reserve(vertexNumber + 1);
		visited_->clear();
	}

	void findPath(my_graph::vertex_id startVertex, my_graph::vertex_id endVertex){
		boost::function<my_graph::edge_weight(Vertex const &)> h =(boost::bind(getEstimate_, _1, graph_->get_vertex(endVertex)));

		this->priorityQueue_.insert(0.0, startVertex);
		(*visited_)[startVertex] = my_graph::path_vertex(startVertex, 0.0);
		
		QueueVertex heapVertex;						//priorityQueue_ top 
		Vertex const *v = 0;								//curent vertex 
		Vertex const *u = 0;								//conjugate vertex for v
		edge_weight weightVU = 0.0f;		//lenght of directed edge (v,u)
		edge_weight estimate = 0.0f;
		my_graph::vertex_id vId = -1;				//v->get_id();
		my_graph::vertex_id uId = -1;				//u->get_id();

		while(this->priorityQueue_.isEmpty() == false) {
			
			heapVertex = priorityQueue_.pop();		//deleteMin
			if(heapVertex.id == endVertex)			//if found path to target
				break;

	
			v = &(graph_->get_vertex(heapVertex.id));
			vId = v->get_id();
			
	
			for(Vertex::ve_const_iterator iter = v->out_begin(); iter != v->out_end(); ++iter) {
				u = &((*iter)->get_v2());
				uId = u->get_id();
				weightVU = getWeight_(*(*iter));
				estimate = h(graph_->get_vertex(uId));

				if(visited_->count(uId) == 0) {
					//здесь пусть хранится только g() h мы всегда можем посчитать
					(*visited_)[uId] = my_graph::path_vertex(uId, 
														(*visited_)[vId].d + weightVU,
														(*iter)->get_id());
					//а вот сюда положем (g + h)
					priorityQueue_.insert((*visited_)[uId].d + estimate, uId);
					
				}else if((*visited_)[uId].d > ((*visited_)[vId].d + weightVU) ) {
					(*visited_)[uId].d  = (*visited_)[vId].d + weightVU;
					(*visited_)[uId].inc = (*iter)->get_id();	
					//а вот сюда положем (g + h) да это круто!
					if(priorityQueue_.isInQueue(uId))
						priorityQueue_.decreaseKey((*visited_)[uId].d + estimate, uId);
					else
						priorityQueue_.insert((*visited_)[uId].d + estimate, uId);
				}
										   
			}//for(Vertex::ve_const_iterator iter = v->out_begin(); iter != v->out_end(); ++iter) {
		}//while(this->priorityQueue_.isEmpty() == false) 
	}
};

	using namespace my_graph;

	template <typename VertexData, typename EdgeData>
	void runAStar(const typename AStar<VertexData, EdgeData>::WeightFunction& getWeight,
					   const graph_base<VertexData, EdgeData> &graph,
					   vertex_id startVertex,
					   vertex_id endVertex,
					   path_map *visited1, 
					   path_map *visited2, 
					   path_map *shortestPath) {
		// поиск кратчайшего растояния до вершины
						   AStar<VertexData, EdgeData> dijkstra(&graph, getWeight, visGraphDistance, visited1);
		dijkstra.findPath(startVertex, endVertex);

		//если не достигли цели то то всё
		if(visited1->count(endVertex) == 0)
			return;
		//return;
		//если достигли востанавливаем кратчайший путь.
		my_graph::path_vertex pathVertex = (*visited1)[endVertex];
		AStar<VertexData, EdgeData>::Vertex const * graphVertex = 0; 

		while(pathVertex.inc.is_initialized()) {
			(*shortestPath)[pathVertex.id] = pathVertex;
			graphVertex = &(graph.get_edge((*pathVertex.inc)).get_v1());
			pathVertex = (*visited1)[graphVertex->get_id()];
		}
		(*shortestPath)[startVertex] = (*visited1)[startVertex];
	}

}

#endif