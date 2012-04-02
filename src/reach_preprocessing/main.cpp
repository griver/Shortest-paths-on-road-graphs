// reach_preprocessing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../visualizer_client/vis_graph.h"
#include "old_loader.h"
#include "reach_preprocessor.h"

using my_reaches::reach_map;
using my_reaches::calculate_reaches;

int main(int argc, char* argv[])
{
	string filename = "USA-road-d.NY";
    
    vis_graph g;
    my_graph::load_graph (filename + ".co", g);
    my_graph::load_graph (filename + ".gr", g);

    my_reaches::reach_map reaches;
    calculate_reaches(g, &reaches);

    cout << endl << "Reaches:" << endl;
    for (reach_map::const_iterator it = reaches.begin(); it != reaches.end(); ++it)
        cout << it->first << ": " << it->second << endl;

    return 0;
}

