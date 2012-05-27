#include "stdafx.h"
#include "reach_client.h"
#include "../shared/osm_loader.h"

visualizer *create_visualizer(draw_scope **ppscope);


int main(int argc, char* argv[])
{

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

