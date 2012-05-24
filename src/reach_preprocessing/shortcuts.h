#pragma once

struct shortcut
{
    shortcut() {};
    shortcut(edge_id id, size_t n_edges)
        : id (id), verts(n_edges + 1), edges(n_edges)
    {
    
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & id;
        ar & verts;
        ar & edges;
    }
    
    edge_id id;
    vector<vertex_id> verts;
    vector<edge_id> edges;
};

edge_id add_shortcuts(reach_graph &g, size_t degree, vector<shortcut> &dst, edge_id);
