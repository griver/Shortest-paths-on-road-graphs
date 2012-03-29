#pragma once

namespace my_graph
{
    typedef float edge_weight;
    
    struct heap_vertex 
    {
        inline heap_vertex () {}
        inline heap_vertex (vertex_id id, edge_weight d) : id(id), d(d) {}
        inline bool operator< (const heap_vertex& b) const
        {
            return (d < b.d);
        }

        vertex_id id;
        edge_weight d;
    };
    
    struct path_vertex
    {
        inline path_vertex () {}
        inline path_vertex (vertex_id id, edge_weight d) : id(id), d(d) {}
        inline path_vertex (vertex_id id, edge_weight d, edge_id inc) : id(id), d(d), inc(inc) {}
        
        vertex_id id;
        edge_weight d;
        boost::optional<edge_id> inc;
    };

    typedef vertex_indexed<path_vertex>::map path_map;
    
    //template<typename V, typename E>

    /*template <typename V, typename E>
    bool check_edge (const graph_base<V,E> &g, const path_map &m, edge_id id, bool rev = false)
    {
        const edge_base<V,E> &e = g.get_edge (id);
        vertex_id v2_id = rev ? e.get_v1().get_id() : e.get_v2().get_id();

        if (m.count(v2_id) == 0)
            return false;

        path_map::const_iterator it = m.find (v2_id);
        return (it->second.inc == id);
        
        //return (m[v2_id].inc == id);
    }*/

}
