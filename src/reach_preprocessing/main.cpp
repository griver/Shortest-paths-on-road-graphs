#include "stdafx.h"
#include "reach_client.h"

visualizer *create_visualizer(draw_scope **ppscope);

/*void load_osm(const string &path, vis_graph &ref_graph, vis_coord &ref_mins, vis_coord &ref_maxs);

using my_graph::vertex_id;
using my_graph::edge_id;
using my_graph::path_map;

void test_reach_updater(const vis_graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, path_map &ref_out2);
void test_reach_updater_new(const vis_graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, path_map &ref_out2);
void test_reach_tester(const vis_graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, path_map &ref_out2);
void test_candidates(const vis_graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, path_map &ref_out2);
vis_graph *run_reaches_update(const vis_graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, path_map &ref_out2);

visualizer_client *pclientl = NULL;*/

/*
void test_reach_updater2 (const vis_graph &g, 
                        vertex_id start, 
                        vertex_id end, 
                        path_map *pout1, 
                        path_map *pout2, 
                        path_map *ppath)
{
    run_reaches_update(g, start, end, *pout1, *pout2);
}

void test_reach_tester2 (const vis_graph &g, 
                          vertex_id start, 
                          vertex_id end, 
                          path_map *pout1, 
                          path_map *pout2, 
                          path_map *ppath)
{
    run_reaches_update(g, start, end, *pout1, *pout2);
}
*/


int main(int argc, char* argv[])
{
    cout << sizeof(vis_vertex) << ", " << sizeof(vis_edge) << endl;

    if (argc < 2) 
    {
        std::cerr << "Not enough args" << endl;
        return 1;
    }
        
    /*string path = argv[1];
    graph_loader loader = boost::bind (load_osm, path, _1, _2, _3);*/


    draw_scope *pscope = NULL;
    scoped_ptr<visualizer> pvis (create_visualizer(&pscope));

    reach_client cl (argv[1], pvis.get(), pscope);
    pvis->set_client(&cl);

    //loader(*pgraph, mins, maxs);

    //visualizer_client cl (*pgraph, pvis.get(), pscope, mins, maxs);
    /*cl.register_algorithm("Test", [&pgraph, &cl](const vis_graph &g, 
                                     vertex_id start, 
                                     vertex_id end, 
                                     path_map *pout1, 
                                     path_map *pout2, 
                                     path_map *ppath)
    {
        vis_graph *pnew_graph = run_reaches_update(g, start, end, *pout1, *pout2);
        cl.load_graph(*pnew_graph);
        pgraph.reset(pnew_graph);
    });*/
    //cl.register_algorithm("Tes t 2", test_reach_tester2);



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

