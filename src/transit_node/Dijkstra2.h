#ifndef DIJKSTRA_2_H
#define DIJKSTRA_2_H

#include "../shared/common.h"
#include "../shared/common_algorithms/path.h"
#include "../shared/graph.h"
#include "../shared/coord.h"
#include <functional>
#include <map>
#include "MyPriorityQueue.h"


template <class VertexData, class EdgeData>
class Dijkstra2{
public:
	typedef my_graph::graph_base<VertexData, EdgeData>	Graph;			// Our graph parameterized by data types
	typedef my_graph::edge_base<VertexData, EdgeData>	Edge;
	typedef my_graph::vertex_base<VertexData, EdgeData>	Vertex;										
	typedef boost::function<my_graph::edge_weight (Edge const &)> WeightFunction;	//Function gets Edge end returns edge lenght. 

private:
	Graph const  *graph_;
	unordered_map<my_graph::vertex_id, my_graph::path_vertex> helpMap;
	my_graph::path_map* visited_;
	MyPriorityQueue priorityQueue_;
	WeightFunction getWeight_;

public:
	//visited - tree of vertices viewed by our dijkstra
	//visited type - unordered_map<vertex_id, path_vertex>
	Dijkstra2(Graph const *graph, WeightFunction const &getWeight, my_graph::path_map* visited): graph_(graph), getWeight_(getWeight), visited_(visited){
		this->init(graph->v_count());
		
	}

	void init(size_t vertexNumber) {
		this->priorityQueue_.reserve(vertexNumber + 10);	
	}

	void findPath(my_graph::vertex_id startVertex, my_graph::vertex_id endVertex){
		this->priorityQueue_.insert(0.0, startVertex);
		(*visited_)[startVertex] = my_graph::path_vertex(startVertex, 0.0);
		
		QueueVertex heapVertex;						//priorityQueue_ top 
		Vertex const *v = 0;								//curent vertex 
		Vertex const *u = 0;								//conjugate vertex for v
		my_graph::edge_weight weightVU = 0.0f;		//lenght of directed edge (v,u)
		my_graph::path_vertex visitedU;				//information about u vertex
		my_graph::path_vertex visitedV;				//information about v vertex
		my_graph::vertex_id vId = -1;				//v->get_id();
		my_graph::vertex_id uId = -1;				//u->get_id();
		int counter = 0;
		int hundred = 0;
		while(this->priorityQueue_.isEmpty() == false) {
			//try{
			heapVertex = priorityQueue_.pop();		//deleteMin
			if(heapVertex.id == endVertex)			//if found path to target
				break;

			/*if(visited_->size()/1000 >= counter ) {
				counter++;
				std::cout <<"Visited: " << visited_->size() << std::endl 
					<< "In priority Queue: " << priorityQueue_.getSize()
					<< std::endl<< std::endl;

			}*
		
			/*if(visited_->size()/50 >= hundred )
				hundred++;*/
	
			v = &(graph_->get_vertex(heapVertex.id));
			vId = v->get_id();
			
			/*if(counter >= 260)
				std::cout<<vId<<std::endl;
			if(vId == 236219) {
				std::cout << "this is it" << std::endl;
			}*/

			for(Vertex::ve_const_iterator iter = v->out_begin(); iter != v->out_end(); ++iter) {
				u = &((*iter)->get_v2());
				uId = u->get_id();
				weightVU = getWeight_(*(*iter));
				/*/----------------------------------
				float oldDist = 0;
				float newDist = 0;
				if(visited_->count(uId) == 1) {
					oldDist = (*visited_)[uId].d;
					newDist = (*visited_)[vId].d + weightVU;
				}
				*///-----------------------------------
				if(visited_->count(uId) == 0) {
					(*visited_)[uId] = my_graph::path_vertex(uId, 
														(*visited_)[vId].d + weightVU,
														(*iter)->get_id());
					priorityQueue_.insert((*visited_)[uId].d, uId);
					
				} else if((*visited_)[uId].d > ((*visited_)[vId].d + weightVU) ) {
					(*visited_)[uId].d  = (*visited_)[vId].d + weightVU;
					(*visited_)[uId].inc = (*iter)->get_id();
					if(priorityQueue_.isInQueue(uId))
						priorityQueue_.decreaseKey((*visited_)[uId].d , uId);
					else
						priorityQueue_.insert((*visited_)[uId].d , uId);
				}
										   
			}//for(Vertex::ve_const_iterator iter = v->out_begin(); iter != v->out_end(); ++iter) {
			/*}catch(...) {
				std::cout << "Vertex ID: " <<vId << std::endl;
			}*/
		}//while(this->priorityQueue_.isEmpty() == false) 
	}
	
};

using namespace my_graph;

template <typename VertexData, typename EdgeData>
void run_dijkstra2(const typename Dijkstra2<VertexData, EdgeData>::WeightFunction& getWeight,
				   const graph_base<VertexData, EdgeData> &graph,
				   vertex_id startVertex,
				   vertex_id endVertex,
				   path_map *visited1, 
				   path_map *visited2, 
				   path_map *shortestPath) {
	// поиск кратчайшего растояния до вершины
	Dijkstra2<VertexData, EdgeData> dijkstra(&graph, getWeight, visited1);
	dijkstra.findPath(startVertex, endVertex);

	//если не достигли цели то то всё
	if(visited1->count(endVertex) == 0)
		return;
	
	std::cout << "Dijkstra find: " << (*visited1)[endVertex].d << std::endl;
	//если достигли востанавливаем кратчайший путь.
	my_graph::path_vertex pathVertex = (*visited1)[endVertex];
	Dijkstra2<VertexData, EdgeData>::Vertex const * graphVertex = 0; 

	while(pathVertex.inc.is_initialized()) {
		(*shortestPath)[pathVertex.id] = pathVertex;
		graphVertex = &(graph.get_edge((*pathVertex.inc)).get_v1());
		pathVertex = (*visited1)[graphVertex->get_id()];
	}
	(*shortestPath)[startVertex] = (*visited1)[startVertex];
}

#endif