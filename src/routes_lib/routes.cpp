#include "stdafx.h"
#include "routes.h"
#include "../shared/osm_loader.h"
#include "dijkstra.h"


routes::routes(const string &filename)
{
    load_osm(filename, graph, mins, maxs);
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
}

bool routes::get_path(vertex_id s, vertex_id t, vector<vis_coord> &dst)
{
    path_map tree;
    dijkstra<vis_vertex_data, vis_edge_data> d (graph, s, tree);
    while (!d.done())
    {
        const vertex_id id = d.iterate();
        if (id == t)
            break;
    }

    if (tree.count(t) == 0)
        return false;

    dst.clear();
    vertex_id id = t;
    while (unordered_safe_find_const(tree, id).parent.is_initialized())
    {
        const path_vertex &pv = unordered_safe_find_const(tree, id);
        dst.push_back(graph.get_vertex(id).data.c);
        id = *pv.parent;
    }
    std::reverse(dst.begin(), dst.end());
    return true;
}
