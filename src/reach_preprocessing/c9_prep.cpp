#include "stdafx.h"
#include "c9_prep.h"

c9_prep::c9_prep(const reach_graph &g, size_t num_threads)
: pgraph_(&g)
, reaches_(g.v_count(), 0)
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
    vector<vector<edge_weight> > reaches_array(num_threads_);
    for (size_t i = 0; i < num_threads_; ++i)
        reaches_array[i].resize(pgraph_->v_count(), 0);

    int i;
    const int nverts = pgraph_->v_count();

#pragma omp parallel for num_threads(num_threads_)
    for (i = 0; i < nverts; ++i)
    {
        calculate_reaches(i, reaches_array[omp_get_thread_num()]);
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
