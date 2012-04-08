#pragma once

#include "../shared/coord.h"

typedef coord<float> vis_coord;

namespace my_graph
{
    typedef size_t vertex_id;
    typedef size_t edge_id;
    typedef float edge_weight;
}

struct vis_vertex_data
{
    explicit vis_vertex_data (const vis_coord& c) : c(c) {};

    vis_coord c;
    unsigned __int64 orig_id;
};

struct vis_edge_data
{
    explicit vis_edge_data (my_graph::edge_weight len) : len (len) {};

    my_graph::edge_weight len;
};


#include "new_vis_graph/graph.h"
