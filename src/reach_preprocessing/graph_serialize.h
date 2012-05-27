#pragma once

    template<class Archive>
    void my_graph::vert_edge::serialize(Archive & ar, const unsigned int version)
    {
        ar & v;
        ar & e;
    }



    template<typename V, typename E>
    template<class Archive>
    void my_graph::vertex_base<V, E>::serialize(Archive & ar, const unsigned int version)
    {
        ar & id;
        ar & data;
        ar & adj;
    }



    template<typename V, typename E>
    template<class Archive>
    void my_graph::edge_base<V, E>::serialize(Archive & ar, const unsigned int version)
    {
        ar & data;
    }


    template<typename V, typename E>
    template<class Archive>
    void my_graph::graph_base<V, E>::serialize(Archive & ar, const unsigned int version)
    {
        ar & vertices_;
        ar & edges_;
    }
