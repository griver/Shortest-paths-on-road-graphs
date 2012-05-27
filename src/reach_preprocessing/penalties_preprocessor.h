#pragma once

#include "shortcuts.h"

struct reach_results
{
    vector<shortcut> shortcuts;
    vector<edge_weight> reaches;
};

class penalties_preprocessor
{
public:
    penalties_preprocessor(reach_graph *pg, edge_weight epsilon0, size_t num_threads);
    reach_graph* iterate();
    void add_shortcuts();
    void save_reaches_and_shortcurs(const string &filename);

    const vector<edge_weight> &get_reaches() {return reaches_;}
private:
    
    void calculate_reaches();
    void assemble_reaches(const reach_map &reaches);

    void calculate_penalties();
    void filter_graph();
    inline bool is_green(vertex_id id) const;
    void zero_penalties();

private:
    reach_graph* pgraph_;
    edge_id next_edge_id_;
    vector<edge_weight> reaches_;
    vector<shortcut> shortcuts_;

    edge_weight epsilon_;

    const size_t num_threads_;
public:
    vertex_id show1, show2;
};