#pragma once

#include "../visualizer_client/vis_graph.h"

namespace reaches
{
    typedef unordered_map<vertex_id, edge_weight> reach_map;
    void calculate_reaches(const vis_graph& g, reach_map *pdst);
}

