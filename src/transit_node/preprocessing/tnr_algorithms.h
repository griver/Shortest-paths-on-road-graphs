#ifndef TNR_ALGORITHMS_H
#define	TNR_ALGORITHMS_H

#include "../base_algorithm/path_finder.h"
#include "../../tnr_utils/grid_info.h"
#include "node_selector.h"
#include "../../tnr_utils/tnr_utils.h"
#include "../preprocessing/access_node_preprocessing.h"
#include <fstream>

namespace tnr {

	inline void calculate_cell_access_nodes(grid_cell const & cell,
											vertex_set const & border_nodes,
											path_finder &finder,
											node_selector& selector,
											vertex_set & transit_nodes,
											grid_info & info,
											path_map* visited_tree) {
		cout << "---------------------------------------------------------------------------" << endl;
		cout << "process cell:" << cell.x << "  " << cell.y << endl;
		cout << "border_set size: " << border_nodes.size() << endl;
		size_t old_size = transit_nodes.size();
		vertex_set::const_iterator iter = border_nodes.begin();
		vertex_set::const_iterator end = border_nodes.end();

		coordinate_check in_outer = bind(&grid_info::is_in_rectangle, &info, cell.x-5, cell.y-5, cell.x+4, cell.y+4, _1);
		coordinate_check in_inner = bind(&grid_info::is_in_rectangle, &info, cell.x-3, cell.y-3, cell.x+2, cell.y+2, _1);
		coordinate_filter* filter = new coordinate_filter("RectangleFilter", in_outer);
		shared_ptr<queue_filter> checker(filter);

		finder.set_priority_queue_filter(checker);
		selector.set_coordinate_checkers(in_inner, in_outer);
	
		for(iter; iter != end; ++iter) {
			//if(*iter != 38823) 
			//	continue;
			//cout<<"start find with root: " << *iter << endl;
			finder.init_search( (*iter), (*iter));
			while(filter->get_counter() > 0) {
				if(finder.is_intersect()) break;
				finder.iterate();
			}	 
			//cout<<"visited_tree size is: " << visited_tree->size() << "  and root is: " << *iter  << endl; 
			vertex_set const & access_nodes = selector.select(visited_tree, *iter);
			for(vertex_set:: const_iterator i = access_nodes.begin(); i != access_nodes.end(); ++i) {
				transit_nodes.insert((*i));
			}
			
		}
		cout << "transit_nodes size: " << transit_nodes.size()<< endl;
	}
	
	void search_in_rectangle(const graph_t &graph,
					   vertex_id start_vertex,
					   vertex_id end_vertex,
					   path_map *visited_tree, 
					   path_map *shortest_path) {
		vertex_set transit_nodes;	
		tnr::grid_info info(graph, shortest_path, 100, 100);
		info.calculate_border_vertices(graph, shortest_path);
		
		path_finder forward(&graph, get_weight, visited_tree);
		
		grid_cell cell = info.get_cell(graph.get_vertex(start_vertex).data.c);
		coordinate_check in_outer = bind(&grid_info::is_in_rectangle, &info, cell.x-5, cell.y-5, cell.x+4, cell.y+4, _1);
		coordinate_check in_inner = bind(&grid_info::is_in_rectangle, &info, cell.x-3, cell.y-3, cell.x+2, cell.y+2, _1);
		coordinate_filter* filter = new coordinate_filter("RectangleFilter", in_outer);
		shared_ptr<queue_filter> checker(filter);
	
		forward.set_priority_queue_filter(checker);
	
		forward.init_search(start_vertex, start_vertex);		
		while(filter->get_counter() > 0) {
			if(forward.is_intersect()) break;
			forward.iterate();
		}	
		node_selector selector(graph, in_inner, in_outer);
		vertex_set const & access_nodes = selector.select(visited_tree, start_vertex);
		cout << endl << "///////Number of vertex visited buy tree:  " <<visited_tree->size() << endl;
		cout << "///////Number of selected access nodes: " <<access_nodes.size() << endl;
	}


	
	void grid_based_preprocessor(const graph_t &graph,
					   path_map *visited_tree, 
					   path_map *shortest_path) {
		grid_info info(graph, shortest_path, 100, 100);
		info.calculate_border_vertices(graph, shortest_path);
		vertex_set transit_nodes;

		border_map & grid_borders = info.get_border_map();
		border_map::iterator iter = grid_borders.begin();
		border_map::iterator end = grid_borders.end();
		
		path_finder finder(&graph, get_weight, visited_tree);
		node_selector selector(graph);
		

		for(iter; iter != end; ++iter) {
			//if(iter->first.x == 90 && iter->first.y == 47)
			calculate_cell_access_nodes(iter->first, iter->second, finder, selector, transit_nodes, info, visited_tree);
		}

		cout << "RESULT SIZE: " << transit_nodes.size();

		std::ofstream outfile ("ireland.tn");
		for(vertex_set::iterator i = transit_nodes.begin(); i != transit_nodes.end(); ++i)
			outfile << (*i) << endl;
		outfile.close();
	}


}//namespace tnr {


#endif