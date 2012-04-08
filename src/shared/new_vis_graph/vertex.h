#pragma once

struct vert_edge
{
    typedef my_graph::vertex_id vertex_id;
    typedef my_graph::edge_id edge_id;
    
    vert_edge (vertex_id v, edge_id e) : v(v), e(e) {};
    
    vertex_id v;
    edge_id e;
};

class vis_vertex
{
public:
    friend class vis_graph;
    typedef vis_vertex_data data_type;
    typedef vector<vert_edge>::const_iterator adj_iterator;

    vis_vertex (const data_type &data) : data(data) {}
    
    adj_iterator in_begin()  const {return adj.begin();}
    adj_iterator out_begin() const {return adj.begin();}
    adj_iterator in_end()    const {return adj.end();}
    adj_iterator out_end()   const {return adj.end();}

          data_type &get_data()       {return data;}
    const data_type &get_data() const {return data;}
public:
    data_type data;
private:
    vector<vert_edge> adj;
};
