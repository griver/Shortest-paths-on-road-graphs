
#include "stdafx.h"
#include "../shared/new_vis_graph.h"
#include "../shared/loader.h"
#include "../visualizer_client/visualizer_client.h"

#include "run_algorithms.h"
#include "tnr_algorithms.h"
#include "tnr_loader.h"
#include "tnr_checkers.h"
#include "create_transit_node_graph.h"


visualizer *create_visualizer(draw_scope **ppscope);

void load_osm(const string &path, vis_graph &ref_graph, vis_coord &ref_mins, vis_coord &ref_maxs);

using my_graph::vertex_id;
using my_graph::edge_id;
using my_graph::path_map;



shared_ptr<tnr::vertex_set> tn = tnr::tnr_loader::load_transit_nodes("..\\..\\transit_nodes_data\\ireland_done.tn");
shared_ptr<tnr::access_map> am = tnr::tnr_loader::load_access_nodes("..\\..\\transit_nodes_data\\tn_ireland_done.an");

void dummy_loader (vis_graph &ref_graph, vis_coord &ref_mins, vis_coord &ref_maxs)
{
    
    vertex_id v0 = ref_graph.add_vertex(vis_vertex_data(vis_coord(0, 0)));
    vertex_id v1 = ref_graph.add_vertex(vis_vertex_data(vis_coord(100, 0)));
    vertex_id v2 = ref_graph.add_vertex(vis_vertex_data(vis_coord(100, 100)));
    vertex_id v3 = ref_graph.add_vertex(vis_vertex_data(vis_coord(0, 100)));

    ref_graph.add_edge(v0, v1, vis_edge_data(100));
    ref_graph.add_edge(v1, v2, vis_edge_data(100));
    ref_graph.add_edge(v2, v3, vis_edge_data(100));
    ref_graph.add_edge(v3, v0, vis_edge_data(100));

    ref_mins = vis_coord(-100, -100);
    ref_maxs = vis_coord(200, 200);
}

//my_part==========================================
void run_astar(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
//	start = 554;
//	end = 1;
	my_algorithm::run_astar(g, start, end, pout1, pout2, ppath);
}


void run_dijkstra(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	//start = 1373059;
	//end = 1373550;
	my_algorithm::run_dijkstra(g, start, end, pout1, pout2, ppath);
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


void run_tn_checking_astar(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	my_algorithm::run_astar(g, start, end, pout1, pout2, ppath);
	path_map::iterator iter = ppath->begin();
	path_map::iterator end_iter = ppath->end();
	for(iter; iter != end_iter; ++iter) {
		if(tn->count((iter->first)) > 0 && (iter->first) != start && (iter->first) != end ) {
			std::cout << "TRUE : way passes through transit nodes" << std::endl;
			return;
		}
	}
	std::cout << "FALSE : There is no any transit nodes on this path" << std::endl;
}

void run_access_node_preprocessing(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	tnr::calculate_access_nodes(g, pout1, tn);
}

void run_an_checking_astar(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	bool success  = tnr::an_checking_dijkstra(my_algorithm::request_data_t(g, 7610, 18657, pout1, pout2, ppath), am);
	if(success)
		std::cerr << "all correct!" << std::endl;
	else
		std::cerr << "ERROR!" << std::endl;
}

void run_access_nodes_check(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	tnr::check_access_nodes(my_algorithm::request_data_t(g, start, end, pout1, pout2, ppath), am);
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
		unordered_map<vertex_id, tnr::access_info>::const_iterator an_iter = iter->second.begin();
		unordered_map<vertex_id, tnr::access_info>::const_iterator an_end_iter = iter->second.end();
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


int main(int argc, char* argv[])
{
    graph_loader loader;
    if (argc < 2)
        loader = dummy_loader;
    else
    {
        string path = argv[1];
        loader = boost::bind (load_osm, path, _1, _2, _3);
    }


    draw_scope *pscope = NULL;
    scoped_ptr<visualizer> pvis (create_visualizer(&pscope));
    scoped_ptr<vis_graph> pgraph(new vis_graph());
    vis_coord mins, maxs;

    loader(*pgraph, mins, maxs);
	
	shared_ptr<vis_graph> tn_graph = tnr::create_tn_graph(*pgraph,am, tn);
	tnr::an_graph_check(am,tn);

    visualizer_client cl (*tn_graph, pvis.get(), pscope, mins, maxs);
    
	cl.register_algorithm("Uni A*", run_astar);
	cl.register_algorithm("Dijkstra A*", run_dijkstra);
	//cl.register_algorithm("Run access nodes preprocessing", run_access_node_preprocessing);
	//cl.register_algorithm("tn checked dijstra", run_tn_checking_astar);
	//cl.register_algorithm("run access nodes checking", run_access_nodes_check);
	//cl.register_algorithm("access node checking dijkstra", run_an_checking_astar);
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

