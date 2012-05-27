#pragma once

#include "../shared/new_vis_graph.h"
#include "../shared/common_algorithms/path.h"


template<typename V, typename E>
class dijkstra
{
public:

    typedef my_graph::vertex_id vertex_id;
    typedef my_graph::edge_id edge_id;
    typedef my_graph::edge_weight edge_weight;
    typedef my_graph::path_map path_map;
    typedef my_graph::path_vertex path_vertex;
    typedef my_graph::heap_vertex heap_vertex;

    typedef my_graph::graph_base<V, E> graph;
    typedef my_graph::vertex_base<V, E> vertex;
    typedef my_graph::edge_base<V, E> edge;
    typedef boost::function<bool(vertex_id)> vert_checker;

    //reach_dijkstra(const graph &ref_graph, vertex_id start, path_map &ref_out);
    inline dijkstra(const graph &ref_graph, vertex_id start, path_map &ref_out, const vert_checker& checker = NULL);
    
    inline bool check_vertex(vertex_id id) const;
    inline bool done();
    inline vertex_id iterate();
    inline vertex_id get_next();
    inline const path_map &get_border() const;

private:
    inline void discard_dublicates();    
    inline edge_weight get_weight(const edge &e) {return e.data.len;}

private:
    const graph *pgraph_;
    path_map *pout_;
    path_map border_;
    priority_queue<heap_vertex> q_;
    vert_checker vert_checker_;
public:
    size_t max_heap_size_;
};

template<typename V, typename E>
dijkstra<V, E>::dijkstra(const graph &ref_graph, vertex_id start, path_map &ref_out, const vert_checker& checker)
: pgraph_(&ref_graph)
, pout_(&ref_out)
, max_heap_size_(0)
, vert_checker_(checker)
{
    q_.push(heap_vertex(start, 0));
    border_[start] = path_vertex(start, 0);
}

template<typename V, typename E>
bool dijkstra<V, E>::done()
{
    discard_dublicates();
    return q_.empty();
}

template<typename V, typename E>
my_graph::vertex_id dijkstra<V, E>::iterate()
{
    if (q_.size() > max_heap_size_)
        max_heap_size_ = q_.size();

    discard_dublicates();

    heap_vertex hv = q_.top();  
    q_.pop();
    
    const path_vertex &pv = (*pout_)[hv.id] = unordered_safe_find_const(border_, hv.id);
    border_.erase(hv.id);

    bool check_result = check_vertex(hv.id);
    
    if (check_result)
    {
        const vertex &v = pgraph_->get_vertex(hv.id);

        for (typename vertex::adj_iterator it = v.out_begin(); it != v.out_end(); ++it)
        {
            const vertex_id &adj_vid = (*it).v;
            const vertex &adj_v = pgraph_->get_vertex(adj_vid);
            const edge_id &eid = (*it).e;
            const edge &e = pgraph_->get_edge(eid);

            if (pout_->count (adj_vid) > 0)
                continue;
            
            const path_vertex pv2 (adj_vid, pv.d + get_weight(e), eid, hv.id);
            const heap_vertex hv2 (adj_vid, pv2.d); 

            if (border_.count(adj_vid) == 0 || 
                unordered_safe_find_const(border_, adj_vid).d > pv2.d)
            {
                q_.push(hv2);
                border_[adj_vid] = pv2;
            }
        }
    }
    return hv.id;
}

template<typename V, typename E>
bool dijkstra<V, E>::check_vertex(vertex_id id) const
{
    if (vert_checker_ == NULL)
        return true;
    return vert_checker_(id);
}

template<typename V, typename E>
my_graph::vertex_id dijkstra<V, E>::get_next()
{
    discard_dublicates();
    return q_.top().id;  
}

template<typename V, typename E>
void dijkstra<V, E>::discard_dublicates()
{
    while ((!q_.empty()) && (pout_->count(q_.top().id) != 0))
        q_.pop();
}

template<typename V, typename E>
const my_graph::path_map & dijkstra<V, E>::get_border() const
{
    return border_;
}

