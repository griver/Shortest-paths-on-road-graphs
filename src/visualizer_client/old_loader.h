#pragma once

namespace my_graph
{
    /*template<typename G>*/
    struct loader
    {
        typedef vis_graph graph;
        void load (const std::string& filename, graph& g);

        loader ();
        ~loader ();

    private:
        void process_line       (const std::string& str);
        void read_vertex        (std::istream& src);
        void read_edge          (std::istream& src);
        void read_reach          (std::istream& src);
        void normalize_verts    ();
    private:
        typedef std::vector<coord<long> > verts_vector;

        graph* pg_;
        verts_vector verts;
        size_t verts_read_, edges_read_, reaches_read_;
        bool abort_;

        double max_relation, min_relation;

        char* buf_;
    public:
        coord<long> mins;
        coord<long> maxs;
    };

    void load_graph ( const std::string& str, vis_graph& g );

    extern coord<long> g_loader_mins, g_loader_maxs;
}

