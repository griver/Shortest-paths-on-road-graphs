#pragma once

#include "../shared/grid.h"


class c9_prep
{
public:
    c9_prep (const reach_graph &g, const grid &gr, size_t num_threads);
    void go();
    void save_reaches(const string &filename);
private:
    void calculate_reaches(vertex_id root, vector<edge_weight> &dst, path_map &tree);
    edge_weight update_reaches_recursive(vertex_id id, vector<edge_weight> &dst, const path_map &tree);
    void assemble_reaches(const vector<edge_weight> &local_reaches);


private:
/*#if !defined USE_OPENMP
    int omp_get_thread_num() const {return 0;}
#endif*/

private:
    const reach_graph *pgraph_;
    vector<edge_weight> reaches_;

    const grid gr_;
    const size_t num_threads_;
};