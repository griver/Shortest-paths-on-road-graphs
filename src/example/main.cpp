
#include "stdafx.h"
#include "../shared/new_vis_graph.h"
#include "../shared/loader.h"
#include "../visualizer_client/visualizer_client.h"
#include "../shared/osm_loader.h"

visualizer *create_visualizer(draw_scope **ppscope);


using my_graph::vertex_id;
using my_graph::edge_id;
using my_graph::path_map;

void run_vis_dijkstra(const vis_graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, path_map &ref_path);
void run_vis_bidijkstra(const vis_graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out1, path_map &ref_out2, path_map &ref_path);
void run_vis_astar(const vis_graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, path_map &ref_path);
void run_vis_biastar(const vis_graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out1, path_map &ref_out2, path_map &ref_path);

void run_vis_dijkstra2 (const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath)
{
    run_vis_dijkstra(g, start, end, *pout1, *ppath);
}
void run_vis_bidijkstra2 (const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath)
{
    run_vis_bidijkstra(g, start, end, *pout1, *pout2, *ppath);
}
void run_vis_astar2 (const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath)
{
    run_vis_astar(g, start, end, *pout1, *ppath);
}
void run_vis_biastar2 (const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath)
{
    run_vis_biastar(g, start, end, *pout1, *pout2, *ppath);
}

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

int main(int argc, char* argv[])
{
    /*graph_loader loader;
    if (argc < 2)
        loader = dummy_loader;
    else
    {
        string path = argv[1];
        loader = boost::bind (load_osm, path, _1, _2, _3);
    }*/


    draw_scope *pscope = NULL;
    scoped_ptr<visualizer> pvis (create_visualizer(&pscope));
    scoped_ptr<vis_graph> pgraph(new vis_graph());
    vis_coord mins, maxs;

    load_osm(argv[1], *pgraph, mins, maxs);

    visualizer_client cl (*pgraph, pvis.get(), pscope, mins, maxs);
    
    cl.register_algorithm("Dijkstra", run_vis_dijkstra2);
    cl.register_algorithm("Bidirectional Dijkstra", run_vis_bidijkstra2);
    cl.register_algorithm("A*", run_vis_astar2);
    cl.register_algorithm("Bidirectional A*", run_vis_biastar2);

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

