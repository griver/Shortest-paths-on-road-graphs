#pragma once

class c9_prep
{
public:
    c9_prep (const reach_graph &g, size_t num_threads);
    void go();
    void save_reaches(const string &filename);
private:
    void calculate_reaches(vertex_id root, vector<edge_weight> &dst);
    void assemble_reaches(const vector<edge_weight> &local_reaches);
private:
    const reach_graph *pgraph_;
    vector<edge_weight> reaches_;
    path_map tree_;

    const size_t num_threads_;
};