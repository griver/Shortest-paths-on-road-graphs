#include "stdafx.h"

#include "routes.h"
#include "../shared/osm_loader.h"
#include "dijkstra.h"



routes::routes(string const  &osm_filename, string const &an_filename, string const &dt_filename)
{
    cout << "loading osm graph..." << endl;
    load_osm(osm_filename, graph, mins, maxs);

    cout << "loading access nodes..." << endl;
    am = tnr::tnr_loader::load_access_nodes(an_filename);

    cout << "loading distance table..." << endl;
    dt = tnr::read_dist_table_from(dt_filename);

    cout << "init grid_info..." << endl;
    grid.reset(new tnr::grid_info(graph, 100, 100));

    cout << "tnr finder init.." << endl;
    finder.reset(new tnr::tnr_path_finder(graph, am, dt, &tree));
}

vertex_id routes::get_vertex_by_coord(const vis_coord& c) const
{
    vertex_id cand = 0;
    double cand_dist = std::numeric_limits<double>::max();
    for (auto it = graph.v_begin(); it != graph.v_end(); ++it)
    {
        const vertex_id id = graph.get_vertex_id(it);

        const vis_coord d = it->data.c - c;
        const double dist = std::max(fabs(d.x), fabs(d.y));
        
        if (dist < cand_dist)
        {
            cand_dist = dist;
            cand = id;
        }
    }
    return cand;
}

inline bool routes::is_far_enouth(vis_coord const & a, vis_coord const & b) {
 	if(abs(grid->get_x_cell(a.x) - grid->get_x_cell(b.x)) > 4) 
		return true;
	if(abs(grid->get_y_cell(a.y) - grid->get_y_cell(b.y)) > 4)
		return true;
	return false;
}


bool routes::get_path(vertex_id s, vertex_id t, vector<vis_coord> &dst)
{

    //path_map tree;
    if(is_far_enouth(graph.get_vertex(s).data.c, graph.get_vertex(t).data.c)) {
        cout << "global search" << endl;
        finder->search(s, t);
    } else {
        cout << "local search" << endl;
        tree.clear();
        dijkstra<vis_vertex_data, vis_edge_data> d (graph, s, tree);
        while (!d.done())
        {
            const vertex_id id = d.iterate();
            if (id == t)
                break;
        }
    }
    // check success
    if (tree.count(t) == 0)
        return false;
    
    cout << "path_size:" << tree.size() << endl;
    cout << "dist to target:" << tree[t].d << endl;
    // build path
    dst.clear();
    vertex_id id = t;
    //while (unordered_safe_find_const(tree, id).parent.is_initialized())
    while(id != s)
    {
        const path_vertex &pv = unordered_safe_find_const(tree, id);
        dst.push_back(graph.get_vertex(id).data.c);
        id = *pv.parent;
    }
    std::reverse(dst.begin(), dst.end());
    return true;
}


