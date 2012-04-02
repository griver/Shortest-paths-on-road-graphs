// reach_preprocessing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../visualizer_client/vis_graph.h"
#include "old_loader.h"


int main(int argc, char* argv[])
{
	string filename = "USA-road-d.NY";
    
    vis_graph g;
    my_graph::load_graph (filename + ".co", g);
    my_graph::load_graph (filename + ".gr", g);

    //rea

    return 0;
}

