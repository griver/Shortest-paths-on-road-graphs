#include "stdafx.h"
#include "../shared/client.h"
#include "../shared/visualizer.h"
#include "visualizer_client.h"
#include "old_loader.h"

visualizer *create_visualizer(draw_scope **ppscope);

visualizer_client::visualizer_client(const std::string &filename)
:   dragging_(false)
,   last_coord_(0,0)
,   draw_lit_(false)
//,   draw_reaches_(false)

{
    pd_.reset(create_visualizer(&pscope_));
    
    coord<long> mins, maxs;

    my_graph::load_graph (filename + ".co", g);
    mins = my_graph::g_loader_mins;
    maxs = my_graph::g_loader_maxs;
    my_graph::load_graph (filename + ".gr", g);
    //my_graph::load_graph (filename + ".re", g);

    std::cout << "Mins: " << mins << "\n";
    std::cout << "Maxs: " << maxs << "\n";

    vb = pd_->create_vb(g.v_count());
    ib = pd_->create_ib(g.e_count());

    ib_lit1 = pd_->create_ib(g.e_count());
    ib_lit2 = pd_->create_ib(g.e_count());
    ib_path = pd_->create_ib(g.e_count());

    build_graph();

    pd_->set_client(this);
}

visualizer_client::~visualizer_client()
{
    std::cout << "Deleting client...\n";
    pd_->free_vb (vb);
    pd_->free_ib (ib);
    pd_->free_ib (ib_lit1);
    pd_->free_ib (ib_lit2);
    pd_->free_ib (ib_path);
}

void visualizer_client::build_graph()
{
    size_t index;

    b_vertex *pv = pd_->lock_vb(vb, 0, g.v_count());

    index = 0;
    for (vis_graph::v_iterator it = g.v_begin(); it != g.v_end(); ++it, ++index)
    {
        vis_vertex_data& data = it->second.get_data();

        pv[index].x = data.c.x;
        pv[index].y = data.c.y;
        pv[index].z = 0;

        int color = 0xFFFF0000;//std::min(static_cast<int>((static_cast<double>(data.reach) / reach_limiter) * 1000.0), 255);

        //pv[index].color = D3DCOLOR_XRGB(color, 128-color/2, 128-color/2);

        data.buffer_index_ = index;
    }

    pd_->unlock_vb (vb);

    b_edge* pe = pd_->lock_ib(ib, 0, g.e_count());


    index = 0;
    for (vis_graph::e_iterator it = g.e_begin(); it != g.e_end(); ++it, ++index)
    {
        vis_edge_data& data = it->second.get_data();

        pe[index].v1 = it->second.get_v1().get_data().buffer_index_;
        pe[index].v2 = it->second.get_v2().get_data().buffer_index_;

        data.buffer_index_ = index;
    }

    pd_->unlock_ib (ib);
}

void visualizer_client::on_mouse_move(int x, int y)
{
    coord<int> screen (x, y);
    if (dragging_)
    {
        pscope_->drag (screen, last_coord_);
        on_paint();
    }
    last_coord_ = screen;
}

void visualizer_client::on_mouse_down (int x, int y, int button)
{
    coord<int> screen (x, y);
    if (button == 0)
    {
        test_hover (screen);
        selected_ = hover_;
        if (selected_)
        {
            const vis_vertex_data &data = g.get_vertex(*selected_).get_data();
            std::cout << "Origin: " << data.orig << ", reach: " << data.reach << "\n";
        }
    }
    else if (button == 1)
    {
        dragging_ = true;
        last_coord_ = screen;
    }
    on_paint();
}

void visualizer_client::on_mouse_up (int x, int y, int button)
{
    coord<int> screen (x, y);
    dragging_ = false;
    last_coord_ = screen;
    on_paint();
}

void visualizer_client::on_key_down(int key)
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
    case '1':
        start_ = selected_;
        break;
    case '2':
        end_ = selected_;
        break;
    }
    on_paint();
}
void visualizer_client::on_wheel (int delta)
{
    pscope_->zoom (pow(1.2f, delta / 120.0f), last_coord_);
    on_paint();
}

void visualizer_client::on_resize(int width, int height)
{
    pd_->resize(width, height);
    on_paint();
}

void visualizer_client::on_paint()
{
    pd_->draw_begin();

    pd_->set_color(0xffc0c0c0);
    pd_->draw_buffers(vb, g.v_count(), ib, g.e_count());

    if (selected_)
    {
        pd_->set_color(0xff000000);
        draw_vertex(*selected_, 3, "");
    }

    if (start_)
    {
        pd_->set_color(0xffff0000);
        draw_vertex(*start_, 2, "start");
    }
    if (end_)
    {
        pd_->set_color(0xff0000ff);
        draw_vertex(*end_, 2, "end");
    }


    pd_->draw_end();
}

void visualizer_client::test_hover( coord<int> c )
{
    for (vis_graph::v_const_iterator it = g.v_begin(); it != g.v_end(); ++it )
    {
        coord<int> d = pscope_->world2screen(it->second.get_data().c);
        if (std::max (abs (d.x - c.x), abs (d.y - c.y)) <= 3)
        {
            hover_.reset (it->second.get_id());
            return;
        }

    }
    hover_.reset();
}


void visualizer_client::draw_vertex( my_graph::vertex_id id, int frame, const std::string& str )
{
    coord<int> vert_coord = pscope_->world2screen(g.get_vertex(id).get_data().c);
    coord<int> eps (frame, frame);
    pd_->draw_rect(vert_coord - eps, vert_coord + eps);
    if (!str.empty())
        pd_->draw_text(vert_coord, str);
}
