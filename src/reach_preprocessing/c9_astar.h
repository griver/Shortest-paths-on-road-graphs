#pragma once

#include "../shared/new_vis_graph.h"
#include "../shared/common_algorithms/path.h"


class c9_astar
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
    typedef boost::function<bool(vertex_id)> vert_checker;

    //c9_astar(const graph &ref_graph, vertex_id start, path_map &ref_out);
    inline c9_astar(const graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, const grid& gr, const vector<edge_weight> &reaches);
    
    inline bool check_vertex(vertex_id id, const path_vertex &pv) const;
    inline bool done();
    inline vertex_id iterate();
    inline vertex_id get_next();
    inline const path_map &get_border() const;
    inline edge_weight get_heur(vertex_id id1, vertex_id id2) const;

private:
    inline void discard_dublicates();    
    inline edge_weight get_weight(const edge &e) {return e.data.len;}

private:
    const graph *pgraph_;
    path_map *pout_;
    path_map border_;
    priority_queue<heap_vertex> q_;
    vert_checker vert_checker_;
    vertex_id start_;
    vertex_id end_;
    const coord<int> start_cell_;

    const grid* pgr_;
    const vector<edge_weight> *preaches_;
public:
    size_t max_heap_size_;
};

c9_astar::c9_astar(const graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, const grid& gr, const vector<edge_weight> &reaches)
: pgraph_(&ref_graph)
, pout_(&ref_out)
, max_heap_size_(0)
//, vert_checker_(checker)
, start_(start)
, end_(end)
, start_cell_(gr.getCell(ref_graph.get_vertex(start).data.c))
, pgr_(&gr)
, preaches_(&reaches)
{
    //start_cell_ = pgr_->getCell(pgraph_->get_vertex(start).data.c);
    q_.push(heap_vertex(start, 0));
    border_[start] = path_vertex(start, 0);
}

bool c9_astar::done()
{
    if (pout_->count(end_) != 0)
        return true;

    discard_dublicates();
    return q_.empty();
}

my_graph::vertex_id c9_astar::iterate()
{
    if (q_.size() > max_heap_size_)
        max_heap_size_ = q_.size();

    discard_dublicates();

    heap_vertex hv = q_.top();  
    q_.pop();
    
    const path_vertex &pv = (*pout_)[hv.id] = unordered_safe_find_const(border_, hv.id);
    border_.erase(hv.id);

    bool check_result = check_vertex(hv.id, pv);
    
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
            const heap_vertex hv2 (adj_vid, pv2.d + get_heur(adj_vid, end_)); 

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

bool c9_astar::check_vertex(vertex_id id, const path_vertex &pv) const
{
    const edge_weight ends_dist = std::min(pv.d, get_heur(id, end_));
    if ((*preaches_)[id] < ends_dist)
        return false;
    else
    {
        cout << (*preaches_)[id] << " >= " << ends_dist << endl;
    }


    const vertex &v = pgraph_->get_vertex(id);
    const coord<int> cell_d = pgr_->getCell(v.data.c) - start_cell_;
    if (std::max(std::abs(cell_d.x), std::abs(cell_d.y)) > 4)
        return false;

    return true;
}

my_graph::vertex_id c9_astar::get_next()
{
    discard_dublicates();
    return q_.top().id;  
}

void c9_astar::discard_dublicates()
{
    while ((!q_.empty()) && (pout_->count(q_.top().id) != 0))
        q_.pop();
}

const my_graph::path_map & c9_astar::get_border() const
{
    return border_;
}

edge_weight c9_astar::get_heur(vertex_id id1, vertex_id id2) const
{
    const vertex &v1 = pgraph_->get_vertex(id1);
    const vertex &v2 = pgraph_->get_vertex(id2);

    const vis_coord d = v2.data.c - v1.data.c;
    return sqrt(d.x * d.x + d.y * d.y);
}

