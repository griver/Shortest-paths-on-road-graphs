
#include "stdafx.h"
#include "../shared/osm_loader.h"
#include "../shared/new_vis_graph.h"
#include "../shared/loader.h"
#include "../visualizer_client/visualizer_client.h"

#include "base_algorithm/run_algorithms.h"
#include "preprocessing/tnr_algorithms.h"
#include "../tnr_utils/tnr_loader.h"
#include "checker/tnr_checkers.h"
#include "preprocessing/distance_table_preprocessor.h"
#include "tnr_path_finder.h"
#include "../shared/perfcounter.h"

#include "preprocessing/shortcut/shortcut.h"
#include "checker/access_nodes_checkers.h"


visualizer *create_visualizer(draw_scope **ppscope);

using my_graph::vertex_id;
using my_graph::edge_id;
using my_graph::path_map;

shared_ptr<tnr::vertex_set> tn;
shared_ptr<tnr::dist_table> dt;
shared_ptr<tnr::access_map> am;
shared_ptr<tnr::grid_info> grid;


//my_part==========================================
void run_astar(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {

	double spend;
	perf_counter counter;
	base_algorithm::run_astar(g, start, end, pout1, pout2, ppath);
	spend = counter.time_ms();
	cout << spend << " ms is running time " << endl;
	cout <<"dist to target: " << (*ppath)[end].d << endl; 
}


void run_dijkstra(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {

	double spend;
	perf_counter counter;
	base_algorithm::run_dijkstra(g, start, end, pout1, pout2, ppath);
	spend = counter.time_ms();
	cout << spend << " ms is running time " << endl;
	cout <<"dist to target: " << (*ppath)[end].d << endl;
}

bool is_far_enouth(tnr::coord_t const & a, tnr::coord_t const & b) {
	if(abs(grid->get_x_cell(a.x) - grid->get_x_cell(b.x)) > 4) 
		return true;
	if(abs(grid->get_y_cell(a.y) - grid->get_y_cell(b.y)) > 4)
		return true;
	return false;
}

void tnr_find(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {	

	tnr::tnr_path_finder finder(g,am, dt, ppath);
	
	/*if(!is_far_enouth(g.get_vertex(start).data.c, g.get_vertex(end).data.c)) {
		cout << "to close!!" << endl;
		return;
	}
	cout << " far enough... " << endl;	*/
	double spend;
	perf_counter counter;
	finder.search(start, end);
	spend = counter.time_ms();
	cout << spend << " ms is running time " << endl;
	cout <<"dist to target: " << (*ppath)[end].d << endl;

}

void an_checking_dijkstra(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	//start = 1144;
	//end = 25846;
	double spend;
	perf_counter counter;
	bool success  = tnr::an_checking_dijkstra(base_algorithm::request_data_t(g, start, end, pout1, pout2, ppath), am);
	spend = counter.time_ms();
	cout << spend << " ms is running time " << endl;
	cout <<"dist to target: " << (*ppath)[end].d << endl;	

	if(success)
		std::cerr << "all correct!" << std::endl;
	else
		std::cerr << "ERROR!" << std::endl;
	
}

void calculate_grid_info(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	tnr::grid_info info(g, ppath, 100, 100);
	std::cout << std::endl <<"Sizes: " << info.get_height() << "  " << info.get_width() << std::endl;
	std::cout << "vertical from: " << info.get_lows().x << " to: " << info.get_highs().x << std::endl;
	std::cout << "horisontal from: " << info.get_lows().y << " to: " << info.get_highs().y << std::endl;
	std::cout << "Steps x: " << info.get_x_step() << "  y: " << info.get_y_step() << std::endl;
	std::cout << " NAMBER OF BORDER EDGES: " << ppath->size() << std::endl;	
}


void search_in_rectangle(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	tnr::search_in_rectangle(g, 5352, end, pout1, ppath);
}


void run_preprocessor(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	tnr::grid_based_preprocessor(g, pout1, ppath);
}




void run_access_node_preprocessing(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	//tnr::calculate_access_nodes(g, pout1, tn, "ireland.an");
	tnr::calculate_access_nodes(g, pout1, tn, "ireland.ian");
}


void run_access_nodes_check(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	//tnr::check_access_nodes(base_algorithm::request_data_t(g, start, end, pout1, pout2, ppath), am);
	tnr::check_access_nodes(base_algorithm::request_data_t(g, start, end, pout1, pout2, ppath), am);
}

void run_tnr_check(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	tnr::tnr_path_finder finder(g,am, dt, ppath);
	//tnr::check_tnr_path_finder(base_algorithm::request_data_t(g, start, end, pout1, pout2, ppath), finder);
	tnr::check_tnr_path_finder(base_algorithm::request_data_t(g, start, end, pout1, pout2, ppath), finder);

}


void graph_info(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	cout <<  "vertex number: " << g.v_count() << endl;
	cout <<  "edge number: " << g.e_count() << endl;
	vector<int> edges;
	edges.assign(500, 0);

	for(vis_graph::v_const_iterator iter = g.v_begin(); iter != g.v_end(); ++iter) {
		edges[iter->out_end() - iter->out_begin()]++; 
	}
	
	int null_edges = 0;
	for(vis_graph::e_const_iterator e_iter = g.e_begin(); e_iter != g.e_end(); ++e_iter) {
		if(e_iter->data.len == 0.0f)
			null_edges++;
	}
	cout << "graph has " << null_edges << " with length = 0" << endl;
	for(int i = 0; i < 500; ++i) {
		if(edges[i] != 0) 
			cout <<"number of vertices with " << i << " edges = " << edges[i] << endl;
	}
}

void access_nodes_info(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	tnr::access_map::const_iterator iter = am->begin();
	tnr::access_map::const_iterator end_iter = am->end();

	vector<int> edges;
	edges.assign(500, 0);

	tnr::vertex_set new_tn;
	for(iter; iter != end_iter; ++iter) {
		unordered_map<vertex_id, tnr::access_info>::const_iterator an_iter = iter->begin();
		unordered_map<vertex_id, tnr::access_info>::const_iterator an_end_iter = iter->end();
		for(an_iter; an_iter != an_end_iter; ++an_iter) {
			new_tn.insert(an_iter->first);
		}
	}
	cout << "transit node size: " << tn->size() << endl;
	cout << "transit node size after access nodes: " << new_tn.size() << endl;
	vector<tnr::vertex_id> intersect(tn->size());
	vector<tnr::vertex_id>::iterator it;
	it = std::set_intersection((*tn).begin(), (*tn).end(), new_tn.begin(), new_tn.end(), intersect.begin());
	cout << "and intersection is : " << it - intersect.begin() << endl;
	
	for(tnr::vertex_set::const_iterator tn_iter = tn->begin(); tn_iter != tn->end(); ++tn_iter) {
		int count = (int)(g.get_vertex((*tn_iter)).out_end() - g.get_vertex((*tn_iter)).out_begin());
		edges[count]++;
	}
	
	for(int i = 0; i < 500; ++i) {
		if(edges[i] != 0) 
			cout <<"number of vertices with " << i << " edges = " << edges[i] << endl;
	}

}

//=================================================
void add_route(tnr::graph_t & graph, tnr::vertex_id start, tnr::vertex_id end, size_t number) {
	tnr::vertex_t &start_v = graph.get_vertex(start);
	tnr::vertex_t &end_v = graph.get_vertex(end);
	double x_step = (end_v.data.c.x - start_v.data.c.x) / (double)number;
	double y_step = (end_v.data.c.y - start_v.data.c.y) / (double)number;
	double current_x = start_v.data.c.x;
	double current_y = start_v.data.c.y;
	double rand_x;
	double rand_y;
	tnr::vertex_id prev_id = start;
	for(int i = 1; i < number; ++i) {
		current_x += x_step;
		current_y += y_step;

		rand_x = (std::max(y_step, x_step)/20.0)*((double)((rand() % 11) - 5));
		rand_y = (std::max(y_step, x_step)/20.0)*((double)((rand() % 11) - 5));

		tnr::coord_t coord(current_x + rand_x, current_y + rand_y);
		vertex_id curr_id = graph.add_vertex(tnr::vertex_t::data_type(coord,start_v.data.orig_id + i));
		graph.add_edge(prev_id, curr_id, tnr::edge_t::data_type(1.0));
		prev_id = curr_id;
	}
	graph.add_edge(prev_id, end, tnr::edge_t::data_type(1.0));
}

void create_sample(tnr::graph_t & graph, vis_coord & mins, vis_coord & maxs, int number) {
	srand ( time(NULL) ); 
	tnr::vertex_id up_left = graph.add_vertex(tnr::vertex_t::data_type(tnr::coord_t(50.0, 50.0), 100));
	tnr::vertex_id down_left = graph.add_vertex(tnr::vertex_t::data_type(tnr::coord_t(100.0, 50.0), 200));
	tnr::vertex_id down_right = graph.add_vertex(tnr::vertex_t::data_type(tnr::coord_t(100.0, 100.0), 300));
	tnr::vertex_id up_right = graph.add_vertex(tnr::vertex_t::data_type(tnr::coord_t(50.0, 100.0), 400));
	tnr::vertex_id center = graph.add_vertex(tnr::vertex_t::data_type(tnr::coord_t(75.0, 75.0), 500));
	add_route(graph, up_left, up_right, number);
	add_route(graph, up_left, center, number);
	add_route(graph, up_left, down_left, number);
	add_route(graph, down_left, center, number);
	add_route(graph, down_left, down_right, number);
	add_route(graph, down_right, center, number);
	add_route(graph, down_right, up_right, number);
	add_route(graph, up_right, center, number);
	mins.x = 50.0;
	mins.y = 50.0;
	maxs.x = 100.0;
	maxs.y = 100.0;
}
//================================================================


int main(int argc, char* argv[])
{
    draw_scope *pscope = NULL;
    scoped_ptr<visualizer> pvis (create_visualizer(&pscope));
    scoped_ptr<vis_graph> pgraph(new vis_graph());
    vis_coord mins, maxs;

    load_osm(argv[1], *pgraph, mins, maxs);
	
	//create_sample(*pgraph, mins, maxs, 50);
	tn = tnr::tnr_loader::load_transit_nodes("..\\..\\transit_nodes_data\\ireland.stn");
	am  = tnr::tnr_loader::load_access_nodes("..\\..\\transit_nodes_data\\ireland.sian");
	dt = tnr::read_dist_table_from("..\\..\\transit_nodes_data\\ireland.sidt");
	grid = shared_ptr<tnr::grid_info>(new tnr::grid_info(*pgraph, 100, 100));
	
	/*tnr::distance_table_preprocessor dt_prep(*pgraph, am, tn);
	dt_prep.calculate();
	dt_prep.write_to("ireland.sidt");
	cout << "end of distance table calculation" << endl;
	/*cout << "run shortcut preprocessing" << endl;
	cout << "old graph vertices: " << pgraph->v_count() << endl;
	cout << "old graph edges: " << pgraph->e_count() << endl; 
	tnr::shortcut_preprocessor sh_preprocessor(*pgraph, *grid); 
	shared_ptr <tnr::graph_t> sc_graph = sh_preprocessor.calculate_shourtcuts();
	cout <<"end shortcut preprocessing" << endl;
	cout << "new graph vertices: " << sc_graph->v_count() << endl;
	cout << "new graph edges: " << sc_graph->e_count() << endl;//
	path_map shortest_path, visited_tree;
	tn = sh_preprocessor.calculate_transit_nodes(&visited_tree, &shortest_path);
	cout <<"tn calculation done!" << endl;
	tnr::write_tansit_nodes_to_file("ireland.stn", tn);
	cout << "tn write to file done!" << endl; //*/
	
	visualizer_client cl (*pgraph, pvis.get(), pscope, mins, maxs);
    
	//cl.register_algorithm("grid info", calculate_grid_info);
	cl.register_algorithm("Uni A*", run_astar);
	cl.register_algorithm("Dijkstra A*", run_dijkstra);
	cl.register_algorithm("run tnr search", tnr_find);
	cl.register_algorithm("access node checking dijkstra", an_checking_dijkstra);
	cl.register_algorithm("run tnr check", run_tnr_check);

	//cl.register_algorithm("Run access nodes preprocessing", run_access_node_preprocessing);
	//cl.register_algorithm("tn checked dijstra", run_tn_checking_astar);
	//cl.register_algorithm("run access nodes checking", run_access_nodes_check);
	
	//cl.register_algorithm("graph info", graph_info);
	//cl.register_algorithm("access node info", access_nodes_info);

    MSG msg;
    ZeroMemory( &msg, sizeof( msg ) );
    while( msg.message != WM_QUIT )
    {
        if( GetMessage( &msg, NULL, 0U, 0U/*, PM_REMOVE*/ ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
        }
    }

    return 0;
}

