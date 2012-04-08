#pragma once

struct vis_edge
{
friend class vis_graph;
public:
    typedef vis_edge_data data_type;
public:
    vis_edge(const data_type &data) : data(data) {};

    data_type &get_data()       {return data;}
    const data_type &get_data() const {return data;}
public:
    data_type data;
};
