#pragma once

#include "path.h"
#include "my_heap.h"

namespace my_graph
{


    template <typename V, typename E>
    class dijkstra_class
    {
        typedef graph_base<V,E> graph;

    public:
        typedef boost::function<edge_weight(const edge_base<V,E>&)> weight_fn;

        typedef boost::function<void(edge_id, bool)> relax_callback_fn;

        dijkstra_class (const graph& g, const weight_fn& wfn, path_map &out);

        bool done () const;
        void init (vertex_id first, bool reverse=false, const relax_callback_fn& relax_callback=NULL);
        vertex_id iterate ();

    private:
    
        typedef vertex_indexed<size_t>::map ind_map;

        path_map tree_;

        void update_index (const heap_vertex&, size_t);
        void update_indices ();

        const graph* pg_;
        weight_fn wfn_;
        relax_callback_fn relax_callback_;

        my_heap<heap_vertex> q_;
        ind_map v_indices_;

        bool rev_;
        path_map *pout_;

    };





    template <typename V, typename E>
    dijkstra_class<V,E>::dijkstra_class( const graph& g, const weight_fn& wfn, path_map &out ) 
        :   pg_         (&g) 
        ,   wfn_        (wfn)
        ,   q_          (boost::bind (&dijkstra_class<V,E>::update_index, this, _1, _2))

        ,   v_indices_  (g.v_count())
        ,   pout_        (&out)
    {
        //q_.set_callback();
    }


    template <typename V, typename E>
    void dijkstra_class<V,E>::init (vertex_id start, bool reverse, const relax_callback_fn& relax_callback)
    {
        q_.clear();
        q_.reserve (pg_->v_count());

        v_indices_.clear();
        tree_.clear();
        pout_->clear();

        tree_.rehash (pg_->v_count ());
        v_indices_.rehash (pg_->v_count ());
        rev_ = reverse;
        relax_callback_ = relax_callback;

        path_vertex pv (start, 0);
        heap_vertex hv (start, 0);

        q_.insert(hv);
        tree_[start] = pv;
    };


    template <typename V, typename E>
    vertex_id dijkstra_class<V,E>::iterate ()
    {
        heap_vertex hv = q_.get (0);

        assert (tree_.count(hv.id) > 0);
        const path_vertex &pv = tree_[hv.id];
/*
        path_map::const_iterator it = tree_.find(hv.id);
        assert (it != tree_.end ());
        const path_vertex &pv = it->second;
*/

        q_.pop();

        v_indices_.erase(hv.id);

        const typename graph::vertex& v = pg_->get_vertex(hv.id);

        (*pout_)[hv.id] = pv;


        typename graph::vertex::ve_const_iterator begin, end;
        if (rev_)
        {
            begin = v.in_begin();
            end = v.in_end();
        }
        else
        {
            begin = v.out_begin();
            end = v.out_end();
        }

        for (typename graph::vertex::ve_const_iterator it = begin; it != end; ++it)
        {
            size_t tcc = tree_.size();
            const typename graph::edge& e = *(*it);
            vertex_id v2_id = rev_ ? e.get_v1().get_id () : e.get_v2().get_id ();


            if (pout_->count (v2_id) > 0)
                continue;

            path_vertex pv2 (v2_id, pv.d + wfn_(e), e.get_id());
            heap_vertex hv2 (v2_id, pv.d + wfn_(e));

            if (v_indices_.count (v2_id) != 0)
            {
                if (relax_callback_)
                    relax_callback_(e.get_id(), rev_);

                size_t index = v_indices_[v2_id];
                if (hv2 < q_.get(index))
                {
                    q_.decrease_key(index, hv2);
                    tree_[v2_id] = pv2;
                }
            }
            else
            {
                assert (tree_.count (v2_id) == 0);
                tree_[v2_id] = pv2;
                q_.insert (hv2);
            }

        }

        return hv.id;
    }

    template <typename V, typename E>
    bool dijkstra_class<V,E>::done() const
    {
        return q_.empty();
    }

    template <typename V, typename E>
    void dijkstra_class<V,E>::update_index( const heap_vertex& e, size_t i )
    {
        v_indices_[e.id] = i;

    }

    template <typename V, typename E> 
    edge_weight stub_get_weight (const edge_base<V, E> &e)
    {
        return e.len;
    }


    template <typename V, typename E>
    void run_dijkstra (const graph_base<V, E> &g, vertex_id start, vertex_id end, const typename dijkstra_class<V, E>::weight_fn& wfn, path_map *pout1, path_map *pout2)
    {
        dijkstra_class<V, E> d (g, wfn, *pout1);
        d.init (start);
        while (!d.done())
            d.iterate();
    }

}
