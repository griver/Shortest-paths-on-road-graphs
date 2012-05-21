#pragma once

#include "graph_base.h"

namespace my_graph
{
    template<typename V, typename E>
    class graph_filter
    {
    public:
        typedef typename graph_base<V, E> graph;
        typedef typename vertex_base<V, E> vertex;
        typedef typename edge_base<V, E> edge;

        typedef unordered_map<vertex_id, vertex_id> verts_map;
    public:
        graph_filter() {};

        void set_verts_filter(boost::function<bool(vertex_id)> f)
        {
            verts_filter_ = f;
        }
        void set_edges_filter(boost::function<bool(edge_id)  > f)
        {
            edges_filter_ = f;
        }

        void filter_graph(const graph_base<V, E> &src, graph_base<V, E> &dst);
    private:
        boost::function<bool(vertex_id)> verts_filter_;
        boost::function<bool(edge_id)  > edges_filter_;
        verts_map verts_;
        //vector<edge_id>   *pedges_;
    };

    template<typename V, typename E>
    void my_graph::graph_filter<V, E>::filter_graph(const graph &src, graph &dst)
    {
        verts_.clear();
        
        for (graph::v_const_iterator it = src.v_begin(); it != src.v_end(); ++it)
        {
            const vertex_id id = src.get_vertex_id(it);
            const vertex& v = *it;

            if (verts_filter_ != NULL && !verts_filter_(id))
                continue;

            const vertex_id id1 = dst.add_vertex(v.get_data());
            verts_[id] = id1;

            for (vertex::adj_iterator adj_it = v.out_begin(); adj_it != v.out_end(); ++adj_it)
            {
                if (edges_filter_ != NULL && !edges_filter_(adj_it->e))
                    continue;

                const verts_map::const_iterator v2_it = verts_.find(adj_it->v);
                if (v2_it != verts_.end())
                {
                    const vertex_id id2 = v2_it->second;
                    const edge &e = src.get_edge(adj_it->e);
                    dst.add_edge(id1, id2, e.get_data());
                }
            }
        }
    }


}
