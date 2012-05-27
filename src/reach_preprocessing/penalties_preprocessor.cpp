#include "stdafx.h"
#include "penalties_preprocessor.h"
#include "shortcuts.h"
#include "../shared/graph_filter.h"
#include "reach_updater.h"
#include "reach_dijkstra.h"

#if !defined USE_OPENMP
#define omp_get_thread_num() 0
#endif

penalties_preprocessor::penalties_preprocessor(reach_graph *pg, edge_weight epsilon0, size_t num_threads)
    : pgraph_(pg)
    , next_edge_id_(pg->e_count())
    , reaches_(pg->v_count(), 0)
    , epsilon_(epsilon0)
    , num_threads_(num_threads)
{
    zero_penalties();
}


reach_graph* penalties_preprocessor::iterate()
{
    add_shortcuts();
    calculate_reaches();
    calculate_penalties();
    filter_graph();
    
    epsilon_ *= 3;
    return pgraph_;
}

void penalties_preprocessor::filter_graph()
{
    reach_graph *dst = new reach_graph();

    my_graph::graph_filter<reach_vertex_data, reach_edge_data> filter;
    filter.set_verts_filter([&](my_graph::vertex_id id) -> bool 
    {
        const reach_vertex &v = pgraph_->get_vertex(id);
        return reaches_[v.get_data().orig_id] >= epsilon_;
        //return (true);
    });

    filter.filter_graph(*pgraph_, *dst);

    cout << dst->v_count() << " verts left" << endl;    
    pgraph_= dst;
}

void penalties_preprocessor::add_shortcuts()
{
    next_edge_id_ = ::add_shortcuts(*pgraph_, 3, shortcuts_, next_edge_id_);
}

void penalties_preprocessor::calculate_reaches()
{
    boost::scoped_array<reach_map> local_reaches(new reach_map[num_threads_]);
    boost::scoped_array<size_t> vert_counters(new size_t[num_threads_]);
    std::fill(vert_counters.get(), vert_counters.get() + num_threads_, 0);

    int i;
    const int nverts = pgraph_->v_count();
    

#pragma omp parallel for num_threads(num_threads_)
    for (i = 0; i < nverts; ++i)
    {
        reach_updater updater;
        size_t &counter = vert_counters[omp_get_thread_num()];
        updater.calculate_reaches(*pgraph_, i, epsilon_, local_reaches[omp_get_thread_num()]);

        if (counter % 1000 == 0)
        {
#pragma omp critical
            {
                cout << counter << " verts processed by thread " << omp_get_thread_num() << endl << std::flush;
            }
        }
        ++counter;
    }
    
    for (size_t thread = 0; thread < num_threads_; ++thread)
        assemble_reaches(local_reaches[thread]);
}

void penalties_preprocessor::assemble_reaches(const reach_map &local_reaches)
{
    
    for (auto it = local_reaches.begin(); it != local_reaches.end(); ++it)
    {
        vertex_id orig_id = pgraph_->get_vertex(it->first).get_data().orig_id;
        if (it->second > reaches_[orig_id])
            reaches_[orig_id] = it->second;
    }
}

void penalties_preprocessor::calculate_penalties()
{
    cout << "calculating penalties..." << endl;
    
    path_map tree;
    vector<shortcut> new_shortcurs;
    
    for (auto it = pgraph_->v_begin(); it != pgraph_->v_end(); ++it)
    {
        const vertex_id root_id = pgraph_->get_vertex_id(it);
        if (is_green(root_id))
            continue;

        tree.clear();

        reach_dijkstra d(*pgraph_, root_id, tree, [&](vertex_id id) -> bool 
        {
            return (id == root_id || is_green(id));
        });

        vertex_id pen_cand = root_id;
        while (!d.done())
        {
            const vertex_id new_id = d.iterate();
            const reach_vertex &new_v = pgraph_->get_vertex(new_id);
            if (is_green(new_id))
            {
                const edge_weight d = unordered_safe_find_const(tree, new_id).d + new_v.data.penalty;
                
                if (d > it->data.penalty)
                {
                    it->data.penalty = d;
                    pen_cand = new_v.data.orig_id;
                }
            } 
            else if (new_id != root_id)
            {
                
            }
        }
        /*if (it->data.orig_id == 77722)
        {
            cout << "Penalty at " << pen_cand << endl;
        }*/

        //if ()
    }

    cout << "done" << endl;
}

bool penalties_preprocessor::is_green(vertex_id id) const
{
    const reach_vertex &v = pgraph_->get_vertex(id);
    return reaches_[v.get_data().orig_id] < epsilon_;
}

void penalties_preprocessor::zero_penalties()
{
    for (auto it = pgraph_->v_begin(); it != pgraph_->v_end(); ++it)
        it->data.penalty = 0;
}

void penalties_preprocessor::save_reaches_and_shortcurs(const string &filename)
{
    cout << "saving reaches and shortcuts ...";
    std::ofstream ofs (filename + ".reaches_shortcuts", std::ios_base::out | std::ios_base::binary);
    boost::archive::binary_oarchive ar (ofs);
    ar << reaches_;
    ar << shortcuts_;
    cout << "done" << endl;

}
