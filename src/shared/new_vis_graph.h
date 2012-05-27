#pragma once

#include "../shared/coord.h"
#include "graph_base.h"

typedef coord<double> vis_coord;

#if !defined (DISABLE_VIS)
struct vis_vertex_data
{
    explicit vis_vertex_data (const vis_coord& c) : c(c) {};
    vis_vertex_data (const vis_coord& c, unsigned __int64 osm_id) : c(c), orig_id(osm_id) {};

    vis_coord c;
    unsigned __int64 orig_id;
};

struct vis_edge_data
{
    explicit vis_edge_data (my_graph::edge_weight len) : len (len) {};  

    my_graph::edge_weight len;
};



typedef my_graph::vertex_base<vis_vertex_data, vis_edge_data> vis_vertex;
typedef my_graph::edge_base  <vis_vertex_data, vis_edge_data> vis_edge;
typedef my_graph::graph_base <vis_vertex_data, vis_edge_data> vis_graph;

#endif