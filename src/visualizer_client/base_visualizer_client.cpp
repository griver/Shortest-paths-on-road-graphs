#include "stdafx.h"
#include "../shared/base_visualizer_client.h"


base_visualizer_client::base_visualizer_client(visualizer *pvis, draw_scope *pscope)
    :   dragging_(false)
    ,   last_coord_(0,0)
    ,   pd_(pvis)
    ,   pscope_(pscope)
{

}


void base_visualizer_client::zoom(const coord<float> &mins, const coord<float> &maxs)
{
    float scale = std::max(maxs.x - mins.x, maxs.y - mins.y);
    coord<float> corner (maxs.x, mins.y);
    pscope_->reset_scope(corner, 1000.0f / scale);

    cout << "Mins: " << mins << endl;
    cout << "Maxs: " << maxs << endl;
}


void base_visualizer_client::on_paint()
{
    pd_->draw_begin();
    draw (*pd_, *pscope_);
    pd_->draw_end();
}

void base_visualizer_client::on_mouse_move(int x, int y)
{
    coord<int> screen (x, y);
    if (dragging_)
    {
        pscope_->drag (screen - last_coord_);
        on_paint();
    }
    last_coord_ = screen;
}

void base_visualizer_client::on_mouse_down (int x, int y, int button)
{
    coord<int> screen (x, y);
    if (button == 0)
    {
        dragging_ = true;
        last_coord_ = screen;
    }
    else if (button == 1)
    {
    }
    on_paint();
}

void base_visualizer_client::on_mouse_up (int x, int y, int button)
{
    coord<int> screen (x, y);
    dragging_ = false;
    last_coord_ = screen;
    on_paint();
}

void base_visualizer_client::on_key_down(int key)
{
    const float ZOOM = 1.2f;
    switch (key)
    {
    case VK_PRIOR:
        pscope_->zoom(ZOOM, last_coord_);
        break;
    case VK_NEXT:
        pscope_->zoom(1.0f / ZOOM, last_coord_);
        break;
    }
    on_paint();
}
void base_visualizer_client::on_wheel (int delta)
{
    pscope_->zoom (pow(1.2f, delta / 120.0f), last_coord_);
    on_paint();
}

void base_visualizer_client::on_resize(int width, int height)
{
    //pd_->resize(width, height);
    on_paint();
}

graph_desc base_visualizer_client::upload_graph(const vis_graph &g)
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

        pv[vertex_index].x = data.c.x;
        pv[vertex_index].y = data.c.y;
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

void base_visualizer_client::free_vb(vb_id vb)
{
    pd_->free_vb(vb);
}

void base_visualizer_client::free_ib(ib_id ib)
{
    pd_->free_ib(ib);
}

boost::optional<my_graph::vertex_id> base_visualizer_client::get_vertex(const vis_graph &g, const coord<int> &c)
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

