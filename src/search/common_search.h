#ifndef COMMON_SEARCH_H
#define COMMON_SEARCH_H
/*#include "../transit_node/util/tnr_utils.h"
#include "../transit_node/util/grid_info.h"
#include "../transit_node/tnr_path_finder.h"*/
/*
class common_search {
public: 
	typedef tnr::graph_t graph_t;
	typedef tnr::grid_info grid_info;
	typedef tnr::tnr_path_finder tnr_path_finder;
	typedef tnr::path_map path_map;
	typedef tnr::path_map coord_array;
private: //fields
	graph_t const & graph;
	grid_info* grid;
	tnr_path_finder* external_finder;
	path_map path;
	

public: //constructors
	common_search(graph_t const & graph, dist_filter const & filter): graph(graph), filter(filter){

	};

	common_search(graph_t const & graph, grid_info const & grid, int apart ): graph(graph), grid(grid), apart(apart){
	};

public:
	
private:
	void init_external_search() {
	}
	
	void init_internal_search() {
	}

	bool is_far_enough(coord_t const &start, coord_t const& end) const{
		if(abs(grid->get_x_cell(a.x) - grid->get_x_cell(b.x)) > 4) 
			return true;
		if(abs(grid->get_y_cell(a.y) - grid->get_y_cell(b.y)) > 4)
			return true;
		return false;
	}
	

};
*/

#endif