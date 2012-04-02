
#include "stdafx.h"
#include "../shared/common_algorithms/dijkstra.h"
#include "../visualizer_client/visualizer_client.h"

visualizer *create_visualizer(draw_scope **ppscope);

void run_vis_dijkstra2 (const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath)
{
    run_dijkstra(get_vis_weight, g, start, end, pout1, pout2, ppath);
}

int main(int argc, char* argv[])
{
    if (argc < 2)
        return 0;

    draw_scope *pscope = NULL;
    scoped_ptr<visualizer> pvis (create_visualizer(&pscope));
    visualizer_client cl (argv[1], pvis.get(), pscope);
    
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

