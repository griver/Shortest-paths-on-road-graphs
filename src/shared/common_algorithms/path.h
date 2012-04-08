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
            return (d > b.d);
        }

        vertex_id id;
        edge_weight d;
    };
    
    struct path_vertex
    {
        inline path_vertex () : d(0) {}
        inline path_vertex (vertex_id id, edge_weight d) : /*id(id),*/ d(d) {}
        //inline path_vertex (vertex_id id, edge_weight d, edge_id inc) : /*id(id),*/ d(d), inc(inc) {}
        inline path_vertex (vertex_id id, edge_weight d, edge_id inc, vertex_id parent) : /*id(id),*/ d(d), inc(inc), parent(parent) {}
        
        //vertex_id id;
        edge_weight d;
        boost::optional<edge_id> inc;       // Will probably be removed (KNK)
        boost::optional<vertex_id> parent;  // Redundant unless inc is removed (KNK)
    };

    typedef unordered_map<vertex_id, path_vertex> path_map;
    
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
