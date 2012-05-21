// visualizer_client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "visualizer_client.h"
#include "../shared/common_algorithms/dijkstra.h"

visualizer *create_visualizer(draw_scope **ppscope);
//client *create_client(const std::string &filename, visualizer *pvis, draw_scope *pscope);

int main(int argc, char* argv[])
{
    draw_scope *pscope = NULL;
    scoped_ptr<visualizer> pvis (create_visualizer(&pscope));
    visualizer_client cl ("USA-road-d.NY", pvis.get(), pscope);
    cl.register_algorithm("Dijkstra 2", boost::bind(my_graph::run_dijkstra<vis_vertex_data, vis_edge_data>, _1, _2, _3, get_vis_weight, _4, _5));

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

