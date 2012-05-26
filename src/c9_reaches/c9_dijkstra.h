#pragma once

#include "../shared/new_vis_graph.h"
#include "../shared/common_algorithms/path.h"
#include "../shared/grid.h"


class c9_dijkstra
{
public:

    typedef my_graph::vertex_id vertex_id;
    typedef my_graph::edge_id edge_id;
    typedef my_graph::edge_weight edge_weight;
    typedef my_graph::path_map path_map;
    typedef my_graph::path_vertex path_vertex;
    typedef my_graph::heap_vertex heap_vertex;

    typedef reach_graph graph;
    typedef reach_vertex vertex;
    typedef reach_edge edge;

    //c9_dijkstra(const graph &ref_graph, vertex_id start, path_map &ref_out);
    inline c9_dijkstra(const graph &ref_graph, const grid &gr, vertex_id start, path_map &ref_out);
    
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
    const grid gr_;
    const coord<int> root_cell_;
public:
    size_t max_heap_size_;
};

c9_dijkstra::c9_dijkstra(const graph &ref_graph, const grid &gr, vertex_id start, path_map &ref_out)
: pgraph_(&ref_graph)
, pout_(&ref_out)
, max_heap_size_(0)
, gr_(gr)
, root_cell_(gr.getCell(ref_graph.get_vertex(start).data.c))
{
    q_.push(heap_vertex(start, 0));
    border_[start] = path_vertex(start, 0);
}

bool c9_dijkstra::done()
{
    discard_dublicates();
    return q_.empty();
}

my_graph::vertex_id c9_dijkstra::iterate()
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

        for (vertex::adj_iterator it = v.out_begin(); it != v.out_end(); ++it)
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

bool c9_dijkstra::check_vertex(vertex_id id) const
{
    const vertex &v = pgraph_->get_vertex(id);
    const coord<int> d = gr_.getCell(v.data.c) - root_cell_;
    return std::max(std::abs(d.x), std::abs(d.y)) <= 100;
}

my_graph::vertex_id c9_dijkstra::get_next()
{
    discard_dublicates();
    return q_.top().id;  
}

void c9_dijkstra::discard_dublicates()
{
    while ((!q_.empty()) && (pout_->count(q_.top().id) != 0))
        q_.pop();
}

const my_graph::path_map & c9_dijkstra::get_border() const
{
    return border_;
}

