#pragma once

#include "../visualizer_client/vis_graph.h"

namespace my_reaches
{
    typedef unordered_map<vertex_id, edge_weight> reach_map;
    void calculate_reaches(const vis_graph& g, reach_map *pdst);
    void calculate_reaches_partial(vis_graph& g, my_reaches::reach_map *pdst, edge_weight epsilon);
}

