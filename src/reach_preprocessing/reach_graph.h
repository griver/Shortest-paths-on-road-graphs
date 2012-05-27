#pragma once

#include "../shared/new_vis_graph.h"
#include "../shared/common_algorithms/path.h"

typedef vis_coord reach_coord;
typedef unsigned __int64 osm_vertex_id;

struct reach_vertex_data
{
    reach_vertex_data () {};
    explicit reach_vertex_data (const reach_coord& c) : c(c), penalty(0) {};
    reach_vertex_data (const vis_coord& c, unsigned __int64 osm_id) : c(c), penalty(0) {};


    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & c;
    }

    reach_coord c;
    my_graph::vertex_id orig_id;
    my_graph::edge_weight penalty;
};

typedef unsigned int shortcut_id;

struct reach_edge_data
{
    reach_edge_data () {};
    explicit reach_edge_data (my_graph::edge_weight len) : len(len) {};  

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & len;
    }


    my_graph::edge_weight len;
    my_graph::edge_id orig_id;
};

/*struct reach_shortcut
{

};*/
typedef vector<osm_vertex_id> reach_shortcut;


typedef my_graph::vertex_base<reach_vertex_data, reach_edge_data> reach_vertex;
typedef my_graph::edge_base  <reach_vertex_data, reach_edge_data> reach_edge;
typedef my_graph::graph_base <reach_vertex_data, reach_edge_data> reach_graph;

using my_graph::vertex_id;
using my_graph::edge_id;
using my_graph::edge_weight;
using my_graph::vert_edge;

using my_graph::path_map;
using my_graph::path_vertex;

typedef unordered_map<vertex_id, edge_weight> reach_map;
