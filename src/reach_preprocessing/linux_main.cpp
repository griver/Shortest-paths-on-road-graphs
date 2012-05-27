#include "stdafx.h"
#include "../shared/graph_filter.h"
#include "../shared/osm_loader.h"
#include "penalties_preprocessor.h"
#include "reach_dijkstra.h"
#include "shortcuts.h"

int main(int argc, char* argv[])
{

    if (argc < 2) 
    {
        std::cerr << "Not enough args" << endl;
        return 1;
    }

	
	string filename = argv[1];
    reach_coord mins, maxs;
	
	boost::scoped_ptr<reach_graph> pgraph (new reach_graph());
	load_osm(filename, *pgraph, mins, maxs);
	penalties_preprocessor prep (pgraph.get(), 0.01, 1);
	pgraph.reset (prep.iterate());
	

	return 0;
}