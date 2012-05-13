// new_vis_graph_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../shared/new_vis_graph.h"

int main(int argc, char* argv[])
{
	vis_graph g;
    
    my_graph::vertex_id id1 = g.add_vertex(vis_vertex_data(vis_coord(0, 0)));
    my_graph::vertex_id id2 = g.add_vertex(vis_vertex_data(vis_coord(100, 0)));

    my_graph::edge_id e = g.add_edge(id1, id2, vis_edge_data(100));

    for (vis_graph::v_const_iterator it = g.v_begin(); it != g.v_end(); ++it)
    {
        cout << (*it).data.c << endl;
    }
    
    return 0;
}

