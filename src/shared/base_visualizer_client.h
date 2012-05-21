#pragma once

#include "coord.h"
#include "client.h"
#include "draw_scope.h"
#include "visualizer.h"
#include "new_vis_graph.h"
#include "common_algorithms/path.h"

struct graph_desc 
{
    vb_id vb;
    size_t vb_size;
    ib_id ib;
    size_t ib_size;
};

struct tree_desc
{
    ib_id ib;
    size_t ib_size;
};

struct base_visualizer_client : client
{
    base_visualizer_client (visualizer *pvis, draw_scope *pscope);

    void zoom (const coord<float> &mins, const coord<float> &maxs);

    virtual void on_mouse_move (int x, int y);
    virtual void on_mouse_down (int x, int y, int button);
    virtual void on_mouse_up   (int x, int y, int button);
    virtual void on_key_down   (int key);
    virtual void on_wheel      (int delta);
    virtual void on_resize     (int width, int height);
    virtual void on_paint      ();

    virtual void draw (visualizer &d, draw_scope &scope) = 0;

    template <typename V, typename E>
    boost::optional<my_graph::vertex_id> get_vertex (const my_graph::graph_base<V, E> &g, const coord<int> &coord);

    template <typename V, typename E>
    graph_desc upload_graph (const my_graph::graph_base<V, E> &g);
    
    tree_desc upload_tree (const my_graph::path_map &m, const tree_desc& src_desc);

    void free_vb (vb_id); 
    void free_ib (ib_id);

    visualizer &get_visualizer() {return *pd_;}
    draw_scope &get_scope() {return *pscope_;}

private:
    visualizer *pd_;
    draw_scope *pscope_;
    bool dragging_;
    coord<int> last_coord_;
};

template <typename V, typename E>
boost::optional<my_graph::vertex_id> base_visualizer_client::get_vertex(const my_graph::graph_base<V, E> &g, const coord<int> &c)
{
    const int TOLERANCE = 3;

    boost::optional<my_graph::vertex_id> res;
    for (auto it = g.v_begin(); it != g.v_end(); ++it)
    {
        my_graph::vertex_id id = it - g.v_begin();
        coord<int> d = pscope_->world2screen((*it).get_data().c);
        if (std::max (abs (d.x - c.x), abs (d.y - c.y)) <= TOLERANCE)
        {
            res.reset(id);
            return res;
        }
    }
    return res;
}

template <typename V, typename E>
graph_desc base_visualizer_client::upload_graph(const my_graph::graph_base<V, E> &g)
{
    graph_desc desc;
    desc.vb_size = g.v_count();
    desc.vb = pd_->create_vb(desc.vb_size);
    desc.ib_size = g.e_count();
    desc.ib = pd_->create_ib(desc.ib_size);

    size_t vertex_index;

    b_vertex *pv = pd_->lock_vb(desc.vb, 0, desc.vb_size);
    b_edge *pe = pd_->lock_ib(desc.ib, 0, desc.ib_size);

    vertex_index = 0;
    for (vis_graph::v_const_iterator it = g.v_begin(); it != g.v_end(); ++it, ++vertex_index)
    {
        const vis_vertex &v = *it;
        const vis_vertex_data& data = v.data;

        pv[vertex_index].x = static_cast<float>(data.c.x);
        pv[vertex_index].y = static_cast<float>(data.c.y);
        pv[vertex_index].z = 0;

        for (vis_vertex::adj_iterator e_it = v.out_begin(); e_it != v.out_end(); ++e_it)
        {
            size_t edge_index = (*e_it).e;
            pe[edge_index].v1 = vertex_index;
            pe[edge_index].v2 = (*e_it).v;
        }
    }

    pd_->unlock_ib (desc.ib);
    pd_->unlock_vb (desc.vb);

    return desc;
}