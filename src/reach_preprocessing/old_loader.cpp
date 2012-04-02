#include "stdafx.h"
#include "../visualizer_client/vis_graph.h"
#include "old_loader.h"


namespace my_graph
{
#if defined (NDEBUG)
    const size_t MAX_VERTS = std::numeric_limits<size_t>::max();
#else
    const size_t MAX_VERTS = 10000;
#endif
    const size_t gran = 100000;

    //const size_t buf_size = 1024*1024;

    coord<long> g_loader_mins, g_loader_maxs;


    loader::loader()
    {
        //buf_ = new char[buf_size];
    }
    loader::~loader()
    {
        //delete [] buf_;
    }
    
    void loader/*<G>*/::load( const std::string& filename, graph& g )
    {
        std::ifstream src (filename.c_str());

        if (!src.is_open())
            return;

        //src.rdbuf()->pubsetbuf (buf_, buf_size);

        abort_ = false;
        verts_read_ = edges_read_ = reaches_read_ = 0;
        pg_ = &g;

        mins = coord<long> (std::numeric_limits<long>::max(), std::numeric_limits<long>::max());
        maxs = coord<long> (std::numeric_limits<long>::min(), std::numeric_limits<long>::min());

        max_relation = 0;
        min_relation = std::numeric_limits<float>::max();

        std::string str;
        while (!src.eof() && !abort_)
        {
            std::getline(src, str);
            process_line(str);
        }

        normalize_verts ();
        verts.clear ();

        std::cout << "Relations: " << min_relation << ", " << max_relation << "\n";



    }


    /*template<typename G>*/
    void loader/*<G>*/::normalize_verts()
    {
        for (verts_vector::iterator it = verts.begin (); it != verts.end (); ++it)
        {
            size_t index = it - verts.begin();

            if (index % gran == 0)
            {
                std::cout << index << " verts normalized\n";

            }

            vis_vertex_data data;
            data.orig = *it;
            data.c = (*it) - mins;
            data.c.y = -data.c.y;
            //vertex_id id = pg_->add_vertex (data).get_id ();
			//assert (id == index);
			bool added = pg_->add_vertex (index + 1, data);
			assert (added);
            //std::cout << it - verts.begin() << ": " << *it << /*"; " << mins <<*/ "\n";
        }
    }


    /*template<typename G>*/
    void loader/*<G>*/::process_line( const std::string& str )
    {
        char cmd='i';
        std::stringstream ss (str);
        ss >> cmd;

        if (cmd == 'v')
        {
            read_vertex(ss);
        }
        else if (cmd == 'a')
        {
            read_edge (ss);
        }
        else if (cmd == 'r')
        {
            read_reach (ss);
        }
    }

    /*template<typename G>*/
    void loader/*<G>*/::read_vertex( std::istream& src )
    {
        size_t id;
        coord<long> c;

        src >> id;
        src >> c.x >> c.y;



        //c.y = -c.y;

        if (id >=MAX_VERTS)
        {
            abort_ = true;
            return;
        }

        if (verts_read_ % gran == 0)
        {
            std::cout << verts_read_ << " verts read\n";
        }
        ++verts_read_;
//        std::cout << "Vertex: " << id << "\n";

        if (mins.x > c.x)
            mins.x = c.x;
        if (mins.y > c.y)
            mins.y = c.y;
        
        if (maxs.x < c.x)
            maxs.x = c.x;
        if (maxs.y < c.y)
            maxs.y = c.y;

        verts.push_back(c);
    }

    /*template<typename G>*/
    void loader/*<G>*/::read_edge( std::istream& src )
    {
        size_t v1_id, v2_id;
        long len;

        //src >> id;
        src >> v1_id >> v2_id;
        src >> len;

        if (v1_id>=MAX_VERTS || v2_id >=MAX_VERTS)
        {
            abort_ = true;
            return;
        }

        if (edges_read_ % gran == 0)
        {
            std::cout << edges_read_ << " edges read\n";
        }
        ++edges_read_;

        assert (v1_id != 0 && v2_id != 0);
        // FIXME: Ugly hack! id in file != id in graph
        vis_graph::vertex &v1 = pg_->get_vertex (v1_id);
        vis_graph::vertex &v2 = pg_->get_vertex (v2_id);
        coord<double> c = v1.get_data().c - v2.get_data().c;
        long dist = static_cast<long>(::sqrt (c.x*c.x + c.y*c.y));

        
        
        vis_edge_data data (dist);
        data.orig_len_ = len;

        pg_->add_edge (v1_id, v2_id, data);

        

        double relation = static_cast<double> (len) / dist;
        if (relation > max_relation)
        {
            std::cout << "Max rel: " << relation << "\n";
            max_relation = relation;
        }
        if (relation < min_relation)
        {
            std::cout << "Min rel: " << relation << "\n";
            min_relation = relation;
        }
    }

    void loader/*<G>*/::read_reach( std::istream& src )
    {

        vertex_id id;
        edge_weight reach;
        
        src >> id >> reach;

        if (id>=MAX_VERTS)
        {

            abort_ = true;
            return;
        }

        if (reaches_read_ % 10000 == 0)
        {
            std::cout << reaches_read_ << " reaches read\n";
        }
        ++reaches_read_;

        // FIXME: Ugly hack! id in file != id in graph
        pg_->get_vertex (id).get_data().reach = reach * 1.2;
    }

    void load_graph ( const std::string& str, vis_graph& g )
    {
        loader l;
        l.load(str, g);
        g_loader_mins = l.mins;
        g_loader_maxs = l.maxs;
    }
 
}