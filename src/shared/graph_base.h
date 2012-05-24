#pragma once

namespace my_graph
{
    typedef size_t vertex_id;
    typedef size_t edge_id;
    typedef double edge_weight;

    template<typename V, typename E>
    class graph_base;

    /*
     * --------------------
     * vert_edge
     * --------------------
     */

    struct vert_edge
    {
        typedef my_graph::vertex_id vertex_id;
        typedef my_graph::edge_id edge_id;


        vert_edge (vertex_id v, edge_id e) : v(v), e(e) {};

        vertex_id v;
        edge_id e;

#if defined (SERIALIZE_GRAPH)
        vert_edge () {};

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version);
#endif
    };

    /*
     * --------------------
     * vertex_base
     * --------------------
     */

    template<typename V, typename E>
    class vertex_base
    {
    public:
        friend class graph_base<V, E>;
        typedef V data_type;
        typedef vector<vert_edge>::const_iterator adj_iterator;

        vertex_base (const data_type &data, my_graph::vertex_id id) : data(data), id(id) {}

        adj_iterator in_begin()  const {return adj.begin();}
        adj_iterator out_begin() const {return adj.begin();}
        adj_iterator in_end()    const {return adj.end();}
        adj_iterator out_end()   const {return adj.end();}

        data_type &get_data()       {return data;}
        const data_type &get_data() const {return data;}

        inline size_t get_degree () const {return adj.size();};
        const vector<vert_edge> &get_adj() const {return adj;};

#if defined (SERIALIZE_GRAPH)
        vertex_base () {};
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version);
#endif

    public:
        data_type data;
		my_graph::vertex_id id;
    private:
        vector<vert_edge> adj;
    };

    /*
     * --------------------
     * edge_base
     * --------------------
     */

    template<typename V, typename E>
    struct edge_base
    {
        friend class graph_base<V, E>;
    public:
        typedef E data_type;
    public:
        edge_base(const data_type &data) : data(data) {};

        data_type &get_data()       {return data;}
        const data_type &get_data() const {return data;}

#if defined (SERIALIZE_GRAPH)
        edge_base () {};
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version);
#endif
    public:
        data_type data;
    };


    /*
     * --------------------
     * graph_base
     * --------------------
     */

    template<typename V, typename E>
    class graph_base
    {
    public:
        typedef my_graph::vertex_id vertex_id;
        typedef my_graph::edge_id edge_id;
        typedef vertex_base<V, E> vertex;
        typedef edge_base<V, E> edge;
    public:
        graph_base() {};
    private:
        graph_base(const graph_base&);
        void operator=(const graph_base&);
    public:
        inline       vertex &get_vertex(vertex_id id);
        inline const vertex &get_vertex(vertex_id id) const;
        inline       edge   &get_edge  (edge_id id);
        inline const edge   &get_edge  (edge_id id)   const;

        inline size_t v_count() const;
        inline size_t e_count() const;

        inline vertex_id add_vertex(const typename vertex::data_type &data);
        inline edge_id add_edge(vertex_id v1, vertex_id v2, const typename edge::data_type &data);

        inline void reserve(size_t n_verts, size_t n_edges);
        inline void clear();

    private:
        typedef vector<vertex> vertex_map;
        typedef vector<edge> edge_map;
    public:
        typedef typename vertex_map::iterator v_iterator;
        typedef typename vertex_map::const_iterator v_const_iterator;

        typedef typename edge_map::iterator e_iterator;
        typedef typename edge_map::const_iterator e_const_iterator;

        v_iterator         v_begin    ()        {return vertices_.begin();};
        v_const_iterator   v_begin    () const  {return vertices_.begin();};
        v_iterator         v_end      ()        {return vertices_.end();};
        v_const_iterator   v_end      () const  {return vertices_.end();};

        e_iterator         e_begin    ()        {return edges_.begin();};
        e_const_iterator   e_begin    () const  {return edges_.begin();};
        e_iterator         e_end      ()        {return edges_.end();};
        e_const_iterator   e_end      () const  {return edges_.end();};

        inline vertex_id get_vertex_id(const v_const_iterator &it) const {return it - v_begin();}
        inline vertex_id get_vertex_id(const v_iterator &it) {return it - v_begin();}
        inline edge_id get_edge_id(const e_const_iterator &it) const {return it - e_begin();}
        inline edge_id get_edge_id(const e_iterator &it) {return it - e_begin();}

#if defined (SERIALIZE_GRAPH)
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version);
#endif
    private:
        vertex_map vertices_;
        edge_map edges_;
    };

    template<typename V, typename E>
    typename graph_base<V, E>::vertex &graph_base<V, E>::get_vertex(vertex_id id)
    {
        return vertices_[id];
    }

    template<typename V, typename E>
    typename const graph_base<V, E>::vertex &graph_base<V, E>::get_vertex(vertex_id id) const
    {
        return vertices_[id];
    }

    template<typename V, typename E>
    typename graph_base<V, E>::edge &graph_base<V, E>::get_edge(edge_id id)
    {
        return edges_[id];
    }

    template<typename V, typename E>
    typename const graph_base<V, E>::edge &graph_base<V, E>::get_edge(edge_id id) const
    {
        return edges_[id];
    }

    template<typename V, typename E>
    size_t graph_base<V, E>::v_count() const
    {
        return vertices_.size();
    }

    template<typename V, typename E>
    size_t graph_base<V, E>::e_count() const
    {
        return edges_.size();
    }

    template<typename V, typename E>
    my_graph::vertex_id graph_base<V, E>::add_vertex(const typename vertex::data_type &data)
    {	
		size_t size = vertices_.size();
		vertices_.push_back(vertex(data, size));
		return vertices_.size() - 1;
    }

    template<typename V, typename E>
    my_graph::edge_id graph_base<V, E>::add_edge(vertex_id v1, vertex_id v2, const typename edge::data_type &data)
    {
        edges_.push_back(edge(data));
        edge_id e = edges_.size() - 1;

        vertices_[v1].adj.push_back(vert_edge(v2, e));
        vertices_[v2].adj.push_back(vert_edge(v1, e));
        return e;
    }

    template<typename V, typename E>
    void graph_base<V, E>::reserve(size_t n_verts, size_t n_edges)
    {
        vertices_.reserve(n_verts);
        edges_.reserve(n_edges);
    }

    template<typename V, typename E>
    void graph_base<V, E>::clear()
    {
        vertices_.clear();
        edges_.clear();
    }

#if defined (SERIALIZE_GRAPH)
#include "graph_serialize.h"
#endif


}

