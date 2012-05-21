#include "stdafx.h"
#include "../shared/new_vis_graph.h"
#include "../shared/common_algorithms/path.h"

typedef my_graph::vertex_id vertex_id;
typedef my_graph::edge_id edge_id;
typedef my_graph::heap_vertex heap_vertex;
typedef my_graph::path_vertex path_vertex;
typedef my_graph::path_map path_map;
typedef my_graph::edge_weight edge_weight;

class vis_astar
{
public:
    typedef vis_graph graph;
    typedef vis_vertex vertex;
    typedef vis_edge edge;

    vis_astar(const graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, bool bidirectional = false);
    
    bool done() const;
    vertex_id iterate();

private:
    static inline edge_weight get_weight(const edge &e) {return e.data.len;}
    static inline edge_weight get_heur(const vertex &v, const vertex &dst);
    static inline edge_weight get_heur2(const vertex &v, const vertex &src, const vertex &dst);
private:
    const graph *pgraph_;
    path_map *pout_;
    path_map border_;
    priority_queue<heap_vertex> q_;
    bool bidirectional_;
    const vis_vertex &start_vertex_;
    const vis_vertex &end_vertex_;
public:
    size_t max_heap_size_;
};

vis_astar::vis_astar(const graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, bool bidirectional)
: pgraph_(&ref_graph)
, pout_(&ref_out)
, max_heap_size_(0)
, bidirectional_(bidirectional)
, start_vertex_(ref_graph.get_vertex(start))
, end_vertex_(ref_graph.get_vertex(end))
{
    q_.push(heap_vertex(start, 0));
    border_[start] = path_vertex(start, 0);
    //end_coord_ = pgraph_->get_vertex(end).data.c;
}

bool vis_astar::done() const
{
    return q_.empty();
}

inline edge_weight vis_astar::get_heur(const vertex &v, const vertex &dst)
{
    vis_coord d = dst.data.c - v.data.c;
    return sqrt(d.x * d.x + d.y * d.y);
}

inline edge_weight vis_astar::get_heur2(const vertex &v, const vertex &src, const vertex &dst)
{
    return (get_heur (v, dst) - get_heur (v, src)) / 2;
}


vertex_id vis_astar::iterate()
{
    if (q_.size() > max_heap_size_)
        max_heap_size_ = q_.size();

    while (pout_->count(q_.top().id) != 0)
        q_.pop();

    heap_vertex hv = q_.top();
    q_.pop();
    
    const path_vertex &pv = unordered_safe_find_const(border_, hv.id);
    (*pout_)[hv.id] = pv;

    const vertex &v = pgraph_->get_vertex(hv.id);

    for (vertex::adj_iterator it = v.out_begin(); it != v.out_end(); ++it)
    {
        const vertex_id &adj_vid = (*it).v;
        const vertex &adj_v = pgraph_->get_vertex(adj_vid);
        const edge_id &eid = (*it).e;
        const edge &e = pgraph_->get_edge(eid);

        if (pout_->count (adj_vid) > 0)
            continue;
        
        path_vertex pv2 (adj_vid, pv.d + get_weight(e), eid, hv.id);


        edge_weight heap_dist = bidirectional_ ? get_heur2(adj_v, start_vertex_, end_vertex_) : get_heur(adj_v, end_vertex_);
        heap_vertex hv2 (adj_vid, pv2.d + heap_dist);

        if (border_.count(adj_vid) == 0 || 
            unordered_safe_find_const(border_, adj_vid).d > pv2.d)
        {
            q_.push(hv2);
            border_[adj_vid] = pv2;
        }
    }
    border_.erase(hv.id);
    return hv.id;
}


void run_vis_astar(const vis_graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, path_map &ref_path)
{
    vis_astar d(ref_graph, start, end, ref_out);
    while (!d.done())
    {
        if (d.iterate() == end)
            break;
    }

    if (ref_out.count(end) == 0)
        return;

    vertex_id vid = end;
    path_vertex pv = unordered_safe_find_const(ref_out, vid);
    while (pv.parent.is_initialized())
    {
        ref_path[vid] = pv;
        vid = *pv.parent;
        pv = unordered_safe_find_const(ref_out, vid);
    }

    cout << "Max heap: " << d.max_heap_size_ << endl;

    /*vertex_id vid = end;
    const path_vertex &pv = unordered_safe_find_const(ref_out, vid);
    while (pv.inc.is_initialized())
    {
        ref_path[vid] = pv;
        vid = ref_graph(*pv.inc)  g.get_edge(*pv.inc).get_v1().get_id();
        pv = pout1->find(vid)->second;
    }*/

}



void run_vis_biastar(const vis_graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out1, path_map &ref_out2, path_map &ref_path)
{
    vis_astar d[] = 
    {
        vis_astar (ref_graph, start, end, ref_out1, true),
        vis_astar (ref_graph, end, start, ref_out2, true)
    };
    vertex_id id[2];

#pragma omp parallel num_threads(2)
    {
        bool flag = false;
        int counter = 1;

        int n = omp_get_thread_num();
        while (!d[n].done() && !flag)
        {
            id[n] = d[n].iterate();

            if (counter % 50 == 0)
            {
#pragma omp critical
                if (ref_out1.count(id[1]) != 0 || ref_out2.count(id[0]) != 0)
                    flag = true;
            }

            ++counter;
        }
    }



}
