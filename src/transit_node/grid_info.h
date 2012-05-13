#ifndef GRID_INFO_H
#define GRID_INFO_H

#include "stdafx.h"



#include "../shared/new_vis_graph.h"
#include "../shared/common.h"
#include "../shared/common_algorithms/path.h"

#include "tnr_utils.h"

namespace tnr { 
	//usi

	struct grid_info {	
	public: //fields
		grid_coord lows;
		grid_coord highs;
		size_t x_partition;
		size_t y_partition;
		double x_step;
		double y_step;

		border_map border_vertices;
		graph_t const &graph;
		//временно.

	public: //constructors
		grid_info(graph_t const &graph,  int x_partition = 1, int y_partition = 1): graph(graph), x_partition(x_partition), y_partition(y_partition) {
			calculate_limits(graph);
			calculate_steps();
			calculate_border_vertices(graph);
		}

		grid_info(graph_t const &graph, my_graph::path_map *path, int x_partition = 1, int y_partition = 1): graph(graph), x_partition(x_partition), y_partition(y_partition) {
			calculate_limits(graph);
			calculate_steps();
			calculate_border_vertices(graph, path);
			std::cout <<"BORDER VERICES SIZE:" << border_vertices.size() << std::endl;
		}

	private: 
		
		void calculate_border_vertices(graph_t const &graph, path_map* path) {
			border_vertices.clear();
				
			size_t size = graph.v_count();
			vertex_t::adj_iterator iter;
			vertex_t::adj_iterator end;
			grid_cell v_cell, u_cell; 
			vertex_id u_id, v_id;

			for(int i = 0; i < size; ++i) {
				v_id = i;
				vertex_t const &v = graph.get_vertex(v_id);
				grid_cell v_cell = get_cell(v.data.c);
				iter = v.in_begin();
				end = v.in_end();

				for(iter; iter != end; ++iter) {
					u_id = (*iter).v;
					u_cell = get_cell(graph.get_vertex(u_id).data.c);
					if(u_cell != v_cell) {
						border_vertices[u_cell].insert(u_id);
						border_vertices[v_cell].insert(v_id);
						if(path->count(u_id) == 0) 
							(*path)[u_id] = path_vertex(u_id, 6.0f,(*iter).e, v_id);
					}
				}
			}
		}


	private: //private methods 
		void calculate_limits(graph_t const &graph) {
			graph_t::v_const_iterator iter = graph.v_begin();
			
			lows.x = highs.x = (double)(*iter).data.c.x;
			lows.y = highs.y = (double)(*iter).data.c.y;
			
			for(iter; iter!= graph.v_end(); ++iter) {

				double currX = (double)(*iter).data.c.x;
				double currY = (double)(*iter).data.c.y;

				if( currX < lows.x)
					lows.x = currX;
				if(currY < lows.y)
					lows.y = currY;

				if(currX > highs.x)
					highs.x = currX;
				if(currY > highs.y)
					highs.y = currY;
			}
		}

		void calculate_steps() {
			x_step = get_height() / (double)x_partition;
			y_step = get_width() / (double)y_partition;
		}


		void calculate_border_vertices(graph_t const &graph) {
			/*graph_t::v_const_iterator iter = graph.v_begin();
			for(iter; iter!= graph.v_end(); ++iter) {
				grid_coord coord = (*iter).data.c;
				grid_cell cell = this->get_cell(coord);
			
			}*/
			border_vertices.clear();
				
			size_t size = graph.v_count();
			vertex_t::adj_iterator iter;
			vertex_t::adj_iterator end;
			grid_cell v_cell, u_cell; 
			vertex_id u_id, v_id;

			for(int i = 0; i < size; ++i) {
				v_id = i;
				vertex_t const &v = graph.get_vertex(v_id);
				grid_cell v_cell = get_cell(v.data.c);
				iter = v.in_begin();
				end = v.in_end();
				for(iter; iter != end; ++iter) {
					u_id = (*iter).v;
					u_cell = get_cell(graph.get_vertex(u_id).data.c);
					if(u_cell != v_cell) {
						border_vertices[u_cell].insert(u_id);
						border_vertices[v_cell].insert(v_id);
					}
				}
			}
		}

	public: //methods
		void set_partition(size_t vertical, size_t horisontal) {
			x_partition = vertical;
			y_partition = horisontal;
			calculate_steps();
			calculate_border_vertices(graph);
		}
		double get_height() const {
			return abs(highs.x - lows.x);
		}
		double get_width() const {
			return abs(highs.y - lows.y);

		}

		grid_coord const get_sizes() const {
			return grid_coord(get_height(), get_width());
		}

		grid_coord const get_lows() const {
			return lows;
		}

		grid_coord const get_highs() const {
			return highs;
		}
		// координату горизонтальной линии
		double get_x_line(int i) const {
			if(i <= x_partition && i >= 0)
				return lows.x + x_step*((double)i);
			else
				return lows.x;
		}

		double get_y_line(int i) const {
			if(i <= y_partition && i >= 0)
				return lows.y + y_step*((double)i);
			else
				return lows.y;
		}

		int get_x_cell(float x) const {
			double tmp =abs((double)x - lows.x);
			double floor_tmp = floor( tmp / x_step );
			return (int)floor_tmp + 1;
		}

		int get_y_cell(float y) const {
			double tmp =abs((double)y - lows.y);
			double floor_tmp = std::floor( tmp / y_step );
			return (int)floor_tmp + 1;
		}

		grid_cell get_cell(coord_t const &coord) const {
			return grid_cell(get_x_cell(coord.x), get_y_cell(coord.y));
		}

		bool is_in_rectangle(int top, int left, int bottom, int right, coord_t const &coord) const {
			grid_cell cell = get_cell(coord);
			if( top < cell.x && cell.x <= bottom) 
				if(left < cell.y && cell.y <= right)
					return true;
			return false;
		}

		double get_x_step() const {
			return this->x_step;
		}

		double get_y_step() const {
			return this->y_step;
		}
		vertex_set const & get_cell_borders(grid_cell cell) {
			 return border_vertices[cell];
		}
		border_map & get_border_map() {
			return border_vertices;
		}

		virtual ~grid_info() {
			border_vertices.clear();
		}
	};
}
#endif
