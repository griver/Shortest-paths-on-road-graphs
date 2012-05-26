#pragma once

class routes
{
public:
    routes (const string &filename);
    bool get_path(vertex_id s, vertex_id t, vector<vis_coord> &dst);
    


private:
    vertex_id get_vertex_by_coord(const vis_coord& c) const;
private:
    vis_coord mins, maxs;
    vis_graph graph;
};