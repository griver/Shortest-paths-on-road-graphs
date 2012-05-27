#pragma once

class reach_updater
{
public:
    typedef unordered_map<vertex_id, vertex_id> candidate_map;
    typedef unordered_map<vertex_id, edge_weight> threshold_map;


    void calculate_reaches(const reach_graph &graph, vertex_id root, edge_weight epsilon, reach_map &ref_reaches);
    const path_map &get_tree() {return tree_;};
    const candidate_map &get_candidates() {return candidates_;};

private:
    void build_tree();
    void update_reaches_with_tree();
    edge_weight update_reaches_recursive(vertex_id id, edge_weight root_penalty);


private:

    const reach_graph *pgraph_;
    unordered_set<vertex_id> saved_;
    path_map tree_;
    candidate_map candidates_;
    threshold_map thresholds_;
    reach_map *preaches_;
    edge_weight epsilon_;
    vertex_id root_;

    unordered_set<vertex_id> visited;
};