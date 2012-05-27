#pragma once

using my_graph::vertex_id;
using my_graph::edge_id;
using my_graph::path_map;
using my_graph::path_vertex;

class routes
{
public:
    routes (const string &filename);
    bool get_path(vertex_id s, vertex_id t, vector<vis_coord> &dst);
    vertex_id get_vertex_by_coord(const vis_coord& c) const;
    
    const vis_graph &get_graph() {return graph;}
    const vis_coord &get_mins() {return mins;}
    const vis_coord &get_maxs() {return maxs;}

private:
    vis_coord mins, maxs;
    vis_graph graph;
    path_map tree;
};