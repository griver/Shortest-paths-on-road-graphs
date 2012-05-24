#include "stdafx.h"
#include "shortcuts.h"
#include "../shared/graph_filter.h"

class penalties_preprocessor
{
public:
    penalties_preprocessor(reach_graph &g, size_t num_threads);
    void iterate();
private:
    void add_shortcuts();
    void calculate_reaches();
    void calculate_penalties();
    void filter_graph();

private:
    boost::scoped_ptr<reach_graph> pgraph_;
    edge_id next_edge_id_;
    vector<shortcut> shortcuts_;
    vector<edge_weight> reaches_;
    edge_weight epsilon_;

    const size_t num_threads;
};

void penalties_preprocessor::iterate()
{
    add_shortcuts();
    //calculate_reaches();
    //calculate_penalties();
    filter_graph();
    
    epsilon_ *= 3;
}

void penalties_preprocessor::filter_graph()
{
    reach_graph *dst = new reach_graph();

    my_graph::graph_filter<reach_vertex_data, reach_edge_data> filter;
    filter.set_verts_filter([&](my_graph::vertex_id id) -> bool 
    {
        const reach_vertex &v = pgraph_->get_vertex(id);
        return reaches_[v.get_data().orig_id] >= epsilon_;
    });

    filter.filter_graph(*pgraph_, *dst);
    
    pgraph_.reset(dst);
}

void penalties_preprocessor::add_shortcuts()
{
    ::add_shortcuts(*pgraph_, 3, shortcuts_, next_edge_id_);
}
