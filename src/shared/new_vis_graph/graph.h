#pragma once

#include "vertex.h"
#include "edge.h"

class vis_graph
{
public:
    typedef my_graph::vertex_id vertex_id;
    typedef my_graph::edge_id edge_id;
    typedef vis_vertex vertex;
    typedef vis_edge edge;
public:
    vis_graph() {};
private:
    vis_graph(const vis_graph&);
    void operator=(const vis_graph&);
public:
    inline       vertex &get_vertex(vertex_id id);
    inline const vertex &get_vertex(vertex_id id) const;
    inline       edge   &get_edge  (edge_id id);
    inline const edge   &get_edge  (edge_id id)   const;
    
    inline size_t v_count() const;
    inline size_t e_count() const;

    inline vertex_id add_vertex(const vertex::data_type &data);
    inline edge_id add_edge(vertex_id v1, vertex_id v2, const edge::data_type &data);

    inline void reserve(size_t n_verts, size_t n_edges);
    inline void clear();

private:
    typedef vector<vertex> vertex_map;
    typedef vector<edge> edge_map;
public:
    typedef vertex_map::iterator v_iterator;
    typedef vertex_map::const_iterator v_const_iterator;

    typedef edge_map::iterator e_iterator;
    typedef edge_map::const_iterator e_const_iterator;

    v_iterator         v_begin    ()        {return vertices_.begin();};
    v_const_iterator   v_begin    () const  {return vertices_.begin();};
    v_iterator         v_end      ()        {return vertices_.end();};
    v_const_iterator   v_end      () const  {return vertices_.end();};

    e_iterator         e_begin    ()        {return edges_.begin();};
    e_const_iterator   e_begin    () const  {return edges_.begin();};
    e_iterator         e_end      ()        {return edges_.end();};
    e_const_iterator   e_end      () const  {return edges_.end();};
private:
    vertex_map vertices_;
    edge_map edges_;
};

vis_graph::vertex &vis_graph::get_vertex(vertex_id id)
{
    return vertices_[id];
}

const vis_graph::vertex &vis_graph::get_vertex(vertex_id id) const
{
    return vertices_[id];
}

vis_graph::edge &vis_graph::get_edge(edge_id id)
{
    return edges_[id];
}

const vis_graph::edge &vis_graph::get_edge(edge_id id) const
{
    return edges_[id];
}

size_t vis_graph::v_count() const
{
    return vertices_.size();
}

size_t vis_graph::e_count() const
{
    return edges_.size();
}

my_graph::vertex_id vis_graph::add_vertex(const vertex::data_type &data)
{
    vertices_.push_back(vertex(data));
    return vertices_.size() - 1;
}

my_graph::edge_id vis_graph::add_edge(vertex_id v1, vertex_id v2, const edge::data_type &data)
{
    edges_.push_back(edge(data));
    edge_id e = edges_.size() - 1;
   
    vertices_[v1].adj.push_back(vert_edge(v2, e));
    vertices_[v2].adj.push_back(vert_edge(v1, e));
    return e;
}

void vis_graph::reserve(size_t n_verts, size_t n_edges)
{
    vertices_.reserve(n_verts);
    edges_.reserve(n_edges);
}

void vis_graph::clear()
{
    vertices_.clear();
    edges_.clear();
}

