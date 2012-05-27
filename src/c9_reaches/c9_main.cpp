#include "stdafx.h"
#include "../shared/osm_loader.h"
#include "../shared/grid.h"
#include "c9_prep.h"

int main(int argc, char* argv[])
{
    if (argc < 2) 
    {
        std::cerr << "Not enough args" << endl;
        return 1;
    }

    const string filename = argv[1];

    reach_graph graph;
    reach_coord mins, maxs;
    //load_osm(filename, graph, mins, maxs);

    grid gr(mins, maxs, 100, 100);

    c9_prep prep (graph, gr, 2);
    prep.go();
    prep.save_reaches(filename + ".c9_reaches");
    return 0;
}