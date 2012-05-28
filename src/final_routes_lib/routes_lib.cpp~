// routes_lib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "routes.h"

int main(int argc, char* argv[])
{
	vector<vis_coord> path;
    routes r("spb.osm");

    cout << r.get_path(0, 45867, path) << endl;

    for (auto it = path.begin(); it != path.end(); ++it)
    {
        cout << *it << endl;
    }

    return 0;
}

