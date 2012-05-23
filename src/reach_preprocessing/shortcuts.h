#pragma once

struct shortcut
{
    shortcut(edge_id id, size_t n_edges)
        : id (id), verts(n_edges + 1), edges(n_edges)
    {
        //verts.reserve(n_edges + 1);
        //edges.reserve(n_edges);
    }
    
    edge_id id;
    vector<vertex_id> verts;
    vector<edge_id> edges;
};

void add_shortcuts(reach_graph &g, size_t degree, vector<shortcut> &dst, edge_id);
