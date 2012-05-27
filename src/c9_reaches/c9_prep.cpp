#include "stdafx.h"
#include "c9_prep.h"
#include "c9_dijkstra.h"

c9_prep::c9_prep(const reach_graph &g, const grid &gr, size_t num_threads)
: pgraph_(&g)
, reaches_(g.v_count(), 0)
, gr_(gr)
, num_threads_(num_threads)
{

}

void c9_prep::save_reaches(const string &filename)
{
    cout << "saving reaches...";
    std::ofstream ofs (filename, std::ios_base::out | std::ios_base::binary);
    boost::archive::binary_oarchive ar (ofs);
    ar << reaches_;
    cout << "done" << endl;
}

void c9_prep::go()
{
    cout << "Threads: " << num_threads_ << endl;
    vector<vector<edge_weight> > reaches_array(num_threads_);
    vector<path_map> path_maps(num_threads_);
    vector<size_t> counters(num_threads_);
    for (size_t i = 0; i < num_threads_; ++i)
        reaches_array[i].resize(pgraph_->v_count(), 0);

    int i;
    const int nverts = pgraph_->v_count();

#pragma omp parallel for num_threads(num_threads_)
    for (i = 0; i < nverts; ++i)
    {
        calculate_reaches(i, reaches_array[omp_get_thread_num()], path_maps[omp_get_thread_num()]);

        if (counters[omp_get_thread_num()] % 1000 == 0)
        {
#pragma omp critical
            {
                cout << counters[omp_get_thread_num()] << " verts processed by thread " << omp_get_thread_num() << endl << std::flush;
            }
        }
        ++counters[omp_get_thread_num()];

    }


    for (size_t thread = 0; thread < num_threads_; ++thread)
        assemble_reaches(reaches_array[thread]);
}

void c9_prep::assemble_reaches(const vector<edge_weight> &local_reaches)
{
    for (size_t i = 0; i < pgraph_->v_count(); ++i)
    {
        if (local_reaches[i] > reaches_[i])
            reaches_[i] = local_reaches[i];
    }
}

void c9_prep::calculate_reaches(vertex_id root, vector<edge_weight> &dst, path_map &tree)
{
    tree.clear();
    c9_dijkstra d (*pgraph_, gr_, root, tree);
    while (!d.done())
        d.iterate();

    update_reaches_recursive(root, dst, tree);


}

edge_weight c9_prep::update_reaches_recursive(vertex_id id, vector<edge_weight> &dst, const path_map &tree)
{
    const reach_vertex &v = pgraph_->get_vertex(id);
    const path_vertex &pv = unordered_safe_find_const(tree, id);
    const edge_weight depth = pv.d;

 
    edge_weight height = 0;
    bool leaf = true;
    for (reach_vertex::adj_iterator it = v.out_begin(); it != v.out_end(); ++it)
    {
        if (tree.count (it->v) == 0)
            continue;
        
        const path_vertex &pv_child = unordered_safe_find_const(tree, it->v);
        if (!pv_child.parent.is_initialized())
            continue;
        
        if (*(pv_child.parent) != id || *(pv_child.inc) != it->e)
            continue;

        leaf = false;
        edge_weight new_height = update_reaches_recursive (it->v, dst, tree) + pv_child.d - pv.d;
        if (new_height > height)
            height = new_height;
    }


    edge_weight new_reach = std::min(depth, height);

    /*if (pdst->count(id) == 0 || unordered_safe_find_const(*pdst, id) < new_reach)
        (*pdst)[id] = new_reach;*/

    if (dst[id] < new_reach)
        dst[id] = new_reach;

    return height;
}
