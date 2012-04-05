
#include "stdafx.h"
#include "../shared/common_algorithms/dijkstra.h"
#include "../visualizer_client/visualizer_client.h"
#include "../shared/loader.h"

visualizer *create_visualizer(draw_scope **ppscope);

void load_osm(const string &path, vis_graph &ref_graph, vis_coord &ref_mins, vis_coord &ref_maxs);

void run_vis_dijkstra2 (const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath)
{
    run_dijkstra(get_vis_weight, g, start, end, pout1, pout2, ppath);
}

void dummy_loader (vis_graph &ref_graph, vis_coord &ref_mins, vis_coord &ref_maxs)
{
    ref_graph.add_vertex(0, vis_vertex_data(0, 0));
    ref_graph.add_vertex(1, vis_vertex_data(100, 0));
    ref_graph.add_vertex(2, vis_vertex_data(100, 100));
    ref_graph.add_vertex(3, vis_vertex_data(0, 100));

    ref_graph.add_edge(0, 1, vis_edge_data(100));
    ref_graph.add_edge(1, 2, vis_edge_data(100));
    ref_graph.add_edge(2, 3, vis_edge_data(100));
    ref_graph.add_edge(3, 0, vis_edge_data(100));

    ref_mins = vis_coord(-100, -100);
    ref_maxs = vis_coord(200, 200);
}

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
    
    visualizer_client cl (loader, pvis.get(), pscope);
    
    cl.register_algorithm("Yet Another Dijkstra", run_vis_dijkstra2);

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

