
#include "stdafx.h"
#include "../shared/new_vis_graph.h"
#include "../shared/loader.h"
#include "../visualizer_client/visualizer_client.h"
#include "run_algorithms.h"
#include "tnr_utils.h"
#include "tnr_algorithms.h"

visualizer *create_visualizer(draw_scope **ppscope);

void load_osm(const string &path, vis_graph &ref_graph, vis_coord &ref_mins, vis_coord &ref_maxs);

using my_graph::vertex_id;
using my_graph::edge_id;
using my_graph::path_map;



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

	my_algorithm::run_astar(g, start, end, pout1, pout2, ppath);
}

void run_dijkstra(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {

	my_algorithm::run_dijkstra(g, start, end, pout1, pout2, ppath);
}

void testGridInfo(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	TNR::grid_info info(g, pout1, 64, 64);
	std::cout << std::endl <<"Sizes: " << info.get_height() << "  " << info.get_width() << std::endl;
	std::cout << "vertical from: " << info.get_lows().x << " to: " << info.get_highs().x << std::endl;
	std::cout << "horisontal from: " << info.get_lows().y << " to: " << info.get_highs().y << std::endl;
	std::cout << "Steps x: " << info.get_x_step() << "  y: " << info.get_y_step() << std::endl;
	std::cout << " NAMBER OF BORDER EDGES: " << pout1->size() << std::endl;
	
}

void search_in_rectangle(const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath) {
	TNR::search_in_rectangle(g, start, end, pout1, pout2, ppath);
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

    visualizer_client cl (*pgraph, pvis.get(), pscope, mins, maxs);
    
    //cl.register_algorithm("Dijkstra", run_vis_dijkstra2);
    //cl.register_algorithm("Bidirectional Dijkstra", run_vis_bidijkstra2);
    //cl.register_algorithm("A*", run_vis_astar2);
    //cl.register_algorithm("Bidirectional A*", run_vis_biastar2);
	cl.register_algorithm("Uni A*", run_astar);
	cl.register_algorithm("Dijkstra A*", run_dijkstra);
	cl.register_algorithm("test GridInfo", testGridInfo);
	cl.register_algorithm("Rectangle Search", search_in_rectangle);
	

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

