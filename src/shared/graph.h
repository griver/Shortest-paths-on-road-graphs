#pragma once

namespace my_graph
{
    typedef size_t vertex_id;
    typedef size_t edge_id;


        template <typename T>
        struct vertex_indexed
        {
            //typedef dumb_map<T> map;
            typedef unordered_map<my_graph::vertex_id, T> map;
        };


    template <typename V, typename E> struct vertex_base;
    template <typename V, typename E> struct edge_base;
    template <typename V, typename E> class graph_base;



    template<typename V, typename E>
    struct vertex_base
    {
        typedef V vertex_data;
        typedef E edge_data;

        typedef graph_base<V,E> graph;
        typedef edge_base<V,E> edge;

        friend class graph_base<V,E>;

        typedef boost::function<void(edge&)>          iter_edges_fn;

        typedef std::list<edge*> edge_list;
        typedef typename edge_list::iterator ve_iterator;
        typedef typename edge_list::const_iterator ve_const_iterator;

        inline my_graph::vertex_id get_id () const;
        inline vertex_data& get_data ();
        inline const vertex_data& get_data () const;

        void iterate_incoming (iter_edges_fn const& f);
        void iterate_outgoing (iter_edges_fn const& f);

        inline ve_iterator         in_begin  ()         {return in_.begin();}
        inline ve_const_iterator   in_begin  () const   {return in_.begin();}
        inline ve_iterator         in_end    ()         {return in_.end();}
        inline ve_const_iterator   in_end    () const   {return in_.end();}
        inline ve_iterator         out_begin ()         {return out_.begin();}
        inline ve_const_iterator   out_begin () const   {return out_.begin();}
        inline ve_iterator         out_end   ()         {return out_.end();}
        inline ve_const_iterator   out_end   () const   {return out_.end();}
    private:
        vertex_base (vertex_id i, const vertex_data& data);

    private:
        vertex_id id_;
        vertex_data data_;
        edge_list in_, out_;
    };

    template<typename V, typename E>
    struct edge_base
    {
        typedef V vertex_data;
        typedef E edge_data;

        typedef graph_base<V,E> graph;
        typedef vertex_base<V,E> vertex;

        friend class graph_base<V,E>;

        inline my_graph::edge_id get_id () const;
        inline edge_data& get_data ();
        inline const edge_data& get_data () const;
        inline vertex& get_v1 ();
        inline vertex& get_v2 ();
        inline const vertex& get_v1 () const ;
        inline const vertex& get_v2 () const ;

    private:
        edge_base (edge_id i, const edge_data& data, vertex *v1, vertex *v2);

        edge_id id_;
        edge_data data_;
        vertex *v1_, *v2_;
    };

    template <class V, class E> 
    class graph_base
    {
    public:
        typedef V vertex_data;
        typedef E edge_data;

        typedef vertex_base<V,E> vertex;
        typedef edge_base<V,E> edge;

        typedef boost::function<void(vertex&)>          iter_vertices_fn;
        typedef boost::function<void(edge  &)>          iter_edges_fn;
        typedef boost::function<void(vertex&, edge*)>   traversal_fn;


        //typedef typename vertex_indexed<vertex>::map vertex_map;
		typedef unordered_map<vertex_id, vertex> vertex_map;
        typedef typename vertex_map::iterator v_iterator;
        typedef typename vertex_map::const_iterator v_const_iterator;

        //typedef typename vertex_indexed<edge>::map edge_map;
        typedef unordered_map<edge_id, edge> edge_map;
        typedef typename edge_map::iterator e_iterator;
        typedef typename edge_map::const_iterator e_const_iterator;

    public:
        graph_base ();
        ~graph_base ();

        void reserve (size_t nverts, size_t nedges);

        inline vertex  &get_vertex (vertex_id i);
        inline edge    &get_edge   (edge_id i);
        inline const vertex  &get_vertex (vertex_id i) const;
        inline const edge    &get_edge   (edge_id i) const;

        bool edge_exists (vertex_id v_id1, vertex_id v_id2) const;

        inline size_t v_count () const;
        inline size_t e_count () const;

        bool add_vertex		(vertex_id id, const vertex_data &data);
		//vertex  &add_vertex (const vertex_data &data);
        //edge    &add_edge   (vertex &v1, vertex& v2, const edge_data   &data);
        edge    &add_edge   (vertex_id iv1, vertex_id iv2, const edge_data   &data);

        void delete_vertex  (vertex_id i);
        void delete_edge    (edge_id i);

        void clear ();

    public:

        void iterate_vertices   (const iter_vertices_fn& f);
        void iterate_edges      (const iter_edges_fn& f);

        v_iterator         v_begin    ()        {return vertices_.begin();};
        v_const_iterator   v_begin    () const  {return vertices_.begin();};
        v_iterator         v_end      ()        {return vertices_.end();};
        v_const_iterator   v_end      () const  {return vertices_.end();};

        e_iterator         e_begin    ()        {return edges_.begin();};
        e_const_iterator   e_begin    () const  {return edges_.begin();};
        e_iterator         e_end      ()        {return edges_.end();};
        e_const_iterator   e_end      () const  {return edges_.end();};


    private:
        vertex_map vertices_;
        edge_map edges_;
        //vertex_id free_vertex_;
        edge_id free_edge_;

    };



    template <class V, class E>
    bool graph_base<V, E>::edge_exists( vertex_id v_id1, vertex_id v_id2 ) const
    {
        const vertex& v1 = get_vertex(v_id1);
        const vertex& v2 = get_vertex(v_id2);
        typename vertex::ve_const_iterator it;
        for (it = v1.out_begin(); it != v1.out_end(); ++it)
        {
            if ((*it)->v2_ == &v2)
                return true;
        }
        return false;
    }

    template <class V, class E>
    size_t graph_base<V, E>::e_count() const
    {
        return edges_.size();
    }

    template <class V, class E>
    size_t graph_base<V, E>::v_count() const
    {
        return vertices_.size();
    }






    /*  --------------------------------------------------------------------------------
    --- vertex ---
    --------------------------------------------------------------------------------  */

    template <class V, class E>
    vertex_base<V, E>::vertex_base (/*my_graph<V, E>::*/vertex_id i, const vertex_data& data)
        :   id_     (i)
        ,   data_   (data)
    {

    }

    template <class V, class E>
    vertex_id vertex_base<V, E>::get_id() const
    {
        return id_;
    }

    template <class V, class E>
    V& vertex_base<V, E>::get_data()
    {
        return data_;
    }

    template <class V, class E>
    const V& vertex_base<V, E>::get_data() const
    {
        return data_;
    }

    template <class V, class E>
    void vertex_base<V, E>::iterate_incoming (iter_edges_fn const & f )
    {
        for (typename edge_list::iterator it = in_.begin (); it != in_.end (); ++it)
            f (*(*it));
    }

    template <class V, class E>
    void vertex_base<V, E>::iterate_outgoing (iter_edges_fn const & f )
    {
        for (typename edge_list::iterator it = out_.begin (); it != out_.end (); ++it)
            f (*(*it));
    }


    /*  --------------------------------------------------------------------------------
    --- edge ---
    --------------------------------------------------------------------------------  */



    template <class V, class E>
    edge_base<V, E>::edge_base( edge_id i, const edge_data& data, vertex *v1, vertex *v2 )
        :   id_     (i), 
        data_   (data), 
        v1_     (v1),
        v2_     (v2)
    {

    }

    template <class V, class E>
    edge_id edge_base<V, E>::get_id() const
    {
        return id_;
    }

    template <class V, class E>
    E& edge_base<V, E>::get_data()
    {
        return data_;
    }

    template <class V, class E>
    const E& edge_base<V, E>::get_data() const
    {
        return data_;
    }

    template <class V, class E>
    vertex_base<V, E>& edge_base<V, E>::get_v1()
    {
        return *v1_;
    }

    template <class V, class E>
    vertex_base<V, E>& edge_base<V, E>::get_v2()
    {
        return *v2_;
    }

    template <class V, class E>
    const vertex_base<V, E>& edge_base<V, E>::get_v1() const 
    {
        return *v1_;
    }

    template <class V, class E>
    const vertex_base<V, E>& edge_base<V, E>::get_v2() const 
    {
        return *v2_;
    }


    /*  --------------------------------------------------------------------------------
    --- my_graph ---
    --------------------------------------------------------------------------------  */


    template <class V, class E>
    graph_base<V, E>::graph_base () 
        : 
	    //free_vertex_    (0), 
        free_edge_      (0)
    {

    };


    template <class V, class E>
    graph_base<V, E>::~graph_base()
    {
        clear ();
    }


    template <class V, class E>
    vertex_base<V, E>& graph_base<V, E>::get_vertex(vertex_id i)
    {
        return vertices_.find(i)->second;
    }

    template <class V, class E>
    edge_base<V, E>& graph_base<V, E>::get_edge(edge_id i)
    {
        return edges_.find(i)->second;
    }

    template <class V, class E>
    const vertex_base<V, E>& graph_base<V, E>::get_vertex(vertex_id i) const
    {
        return vertices_.find(i)->second;
    }

    template <class V, class E>
    const edge_base<V, E>& graph_base<V, E>::get_edge(edge_id i) const
    {
        return edges_.find(i)->second;
    }

#if 0
	template <class V, class E>
    vertex_base<V, E>& graph_base<V, E>::add_vertex(const vertex_data& data)
    {
//        std::cout << free_vertex_ << "\n" << std::flush;
        std::pair<typename vertex_map::iterator, bool> res = vertices_.insert(typename vertex_map::value_type(free_vertex_, vertex (free_vertex_, data)));
        assert (res.second);
        free_vertex_++;
        return res.first->second;
    }
#endif

	template <class V, class E>
	bool graph_base<V, E>::add_vertex(vertex_id id, const vertex_data& data)
	{
		if (vertices_.find(id) != vertices_.end())
			return false;

		//vertices_[id] = vertex (id, data);
		vertices_.insert(typename vertex_map::value_type(id, vertex (id, data)));
		return true;
	}

    /*
    my_graph<V, E>::edge& my_graph<V, E>::add_edge(my_graph<V, E>::vertex &v1, my_graph<V, E>::vertex &v2, const edge_data& data)
    {
    std::pair<edge_map::iterator, bool> res = edges_.insert (edge_map::value_type(free_edge_, edge (free_edge_, data, &v1, &v2)));
    assert (res.second);
    free_edge_++;

    edge &e = res.first->second;
    v1.out_.push_back(e);
    v2.in_.push_back(e);

    return e;
    }*/

    template <class V, class E>
    edge_base<V, E>& graph_base<V, E>::add_edge(vertex_id iv1, vertex_id iv2, const edge_data& data)
    {
        vertex  *pv1 = &get_vertex(iv1),
            *pv2 = &get_vertex(iv2);

        std::pair<typename edge_map::iterator, bool> res = edges_.insert (typename edge_map::value_type(free_edge_, edge (free_edge_, data, pv1, pv2)));
        assert (res.second);
        free_edge_++;

        edge &e = res.first->second;
        pv1->out_.push_back(&e);
        pv2->in_.push_back(&e);

        return e;
    }

    template <class V, class E>
    void graph_base<V, E>::delete_vertex(vertex_id i)
    {
        struct edge_deleter
        {
            edge_deleter (graph_base *pg) : pg_(pg) {};
            void operator () (edge& e) 
            {
                pg_->delete_edge(e.get_id());
            }

            graph_base *pg_;
        };
        edge_deleter deleter(this);

        vertex& v = get_vertex(i);
        v.iterate_incoming(deleter);
        v.iterate_outgoing(deleter);

        //std::cout << "Removing vertex: " << v.data_ << "\n";

        vertices_.erase(i);
    }

    template <class V, class E>
    void graph_base<V, E>::delete_edge(edge_id i)
    {
        //std::cout << "Removing edge: " << get_edge(i).data_ << "\n";
        edge& e = get_edge(i);
        e.v1_->out_.remove(&e);
        e.v2_->in_.remove(&e);
        edges_.erase(i);
    }

    template <class V, class E>
    void graph_base<V, E>::iterate_vertices( const iter_vertices_fn& f )
    {
        for (typename vertex_map::iterator it=vertices_.begin(); it!=vertices_.end(); ++it)
        {
            f(it->second);
        }

    }
    template <class V, class E>
    void graph_base<V, E>::iterate_edges( const iter_edges_fn& f )
    {
        for (typename edge_map::iterator it=edges_.begin(); it!=edges_.end(); ++it)
        {
            f(it->second);
        }

    }



    template <class V, class E>
    void graph_base<V, E>::clear()
    {
        edges_.clear();
        vertices_.clear();

        free_edge_=0;
        //free_vertex_=0;
    }




    template <class V, class E>
    void  graph_base<V, E>::reserve (size_t nverts, size_t nedges)
    {
        vertices_.rehash(nverts);
        vertices_.rehash(nedges);
    }



/*


    template <class V, class E>
    void graph<V, E>::DFS( vertex_ first_id, const traversal_fn& f )
    {
        typedef std::pair<vertex*, edge*> ve;
        std::set<gid> marked;

        std::stack<ve> Q;
        marked.insert(first_id);
        Q.push(ve(&get_vertex(first_id), NULL));

        while (!Q.empty())
        {
            ve p = Q.top();
            Q.pop ();

            f(*p.first, p.second);

            std::list<edge*> &l = p.first->out_;
            for (std::list<edge*>::iterator it = l.begin(); it!=l.end(); ++it)
            {
                ve p2 ((*it)->v2_, *it);
                if (marked.count(p2.first->id_) == 0)
                {
                    marked.insert (p2.first->id_);
                    Q.push(p2);
                }
            }
        }

    }

    template <class V, class E>
    void graph<V, E>::BFS( gid first_id, const traversal_fn& f )
    {
        typedef std::pair<vertex*, edge*> ve;
        std::set<gid> marked;

        std::queue<ve> Q;
        marked.insert(first_id);
        Q.push(ve(&get_vertex(first_id), NULL));

        while (!Q.empty())
        {
            ve p = Q.front();
            Q.pop ();

            f(*p.first, p.second);

            std::list<edge*> &l = p.first->out_;
            for (std::list<edge*>::iterator it = l.begin(); it!=l.end(); ++it)
            {
                ve p2 ((*it)->v2_, *it);
                if (marked.count(p2.first->id_) == 0)
                {
                    marked.insert (p2.first->id_);
                    Q.push(p2);
                }
            }
        }

    }*/

}
