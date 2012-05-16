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



void base_visualizer_client::free_vb(vb_id vb)
{
    pd_->free_vb(vb);
}

void base_visualizer_client::free_ib(ib_id ib)
{
    pd_->free_ib(ib);
}

tree_desc base_visualizer_client::upload_tree(const my_graph::path_map &m, const tree_desc& src_desc)
{
    tree_desc desc;
    desc.ib_size = m.size();
    desc.ib = pd_->create_ib(desc.ib_size);
    
    b_edge* pe_dst = pd_->lock_ib(desc.ib, 0, desc.ib_size);
    b_edge* pe_src = pd_->lock_ib(src_desc.ib, 0, desc.ib_size);

    size_t index = 0;
    for (my_graph::path_map::const_iterator it = m.begin(); it != m.end (); ++it)
    {
        if (!it->second.inc.is_initialized())
            continue;
        
        my_graph::edge_id inc_id = it->second.inc.is_initialized() ? *(it->second.inc) : 0;

        pe_dst[index] = pe_src[inc_id];
        ++index;
    }

    desc.ib_size = index;

    pd_->unlock_ib(src_desc.ib);
    pd_->unlock_ib(desc.ib);

    return desc;
}



