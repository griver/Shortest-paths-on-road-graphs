#pragma once

#include "../shared/graph.h"
#include "../shared/coord.h"
#include "../shared/common_algorithms/path.h"

struct vis_vertex_data
{
    vis_vertex_data () : reach(std::numeric_limits<my_graph::edge_weight>::max()) {};
    explicit vis_vertex_data (const coord<float>& nc) : c(nc), reach(std::numeric_limits<my_graph::edge_weight>::max()) {};
    vis_vertex_data (float x, float y) : c(x,y), reach(std::numeric_limits<my_graph::edge_weight>::max()) {};
    int buffer_index_;

    coord<float> c;
    coord<long> orig;
    my_graph::edge_weight reach;
    //float x_, y_;
};

struct vis_edge_data
{
    explicit vis_edge_data (my_graph::edge_weight len) : len_ (len) {};
    int buffer_index_;
    my_graph::edge_weight len_;
    long orig_len_;

};

inline my_graph::edge_weight get_vis_weight(const my_graph::edge_base<vis_vertex_data, vis_edge_data> &e)
{
    return e.get_data().len_;
}

typedef my_graph::graph_base<vis_vertex_data, vis_edge_data> vis_graph;
