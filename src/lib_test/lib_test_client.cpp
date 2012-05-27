#include "stdafx.h"
#include "lib_test_client.h"

lib_test_client::lib_test_client(const string &filename, visualizer *pvis, draw_scope *pscope)
  : base_visualizer_client(pvis, pscope)
  , r(filename)
{
    pgraph_ = &r.get_graph();
    g_desc = upload_graph(*pgraph_);
    zoom(r.get_mins(), r.get_maxs());
}

lib_test_client::~lib_test_client()
{

}

void lib_test_client::draw(visualizer &d, draw_scope &scope)
{
    auto draw_vertex = [&] (my_graph::vertex_id id, int frame) 
    {
        coord<int> vert_coord = scope.world2screen(pgraph_->get_vertex(id).get_data().c);
        coord<int> eps (frame, frame);
        d.draw_rect(vert_coord - eps, vert_coord + eps);
    };

    d.set_bg_color(0, 0, 0);

    d.set_color(64, 64, 64);
    d.draw_buffers(g_desc.vb, 0, pgraph_->v_count(), g_desc.ib, 0, pgraph_->e_count());

    if (start_.is_initialized())
    {
        d.set_color(255, 0, 0);
        draw_vertex(*start_, 2);
    }
    if (end_.is_initialized())
    {
        d.set_color(255, 255, 0);
        draw_vertex(*end_, 2);
    }
    if (selected_.is_initialized())
    {
        d.set_color(0, 255, 0);
        draw_vertex(*selected_, 3);
    }

    if (!path_.empty())
    {
        d.set_color(255, 255, 255);
        vis_coord last = path_[0];
        for (size_t i = 1; i < path_.size(); ++i)
        {
            d.draw_line(get_scope().world2screen(last), 
                        get_scope().world2screen(path_[i]));
            last = path_[i];
        }
    }
}

void lib_test_client::on_mouse_down(int x, int y, int button)
{
    //boost::optional<my_graph::vertex_id> hover = //get_vertex(*pgraph_, coord<int>(x, y));
    if (button == 0)
    {
        vertex_id hover = get_vertex_by_coord(get_scope().screen2world(vis_coord(x, y)));
        selected_.reset(hover);
    }
    base_visualizer_client::on_mouse_down(x, y, button);
}

void lib_test_client::on_key_down(int key)
{
    switch (key)
    {
    case '1':
        start_ = selected_;
        break;
    case '2':
        end_ = selected_;
        break;
    case VK_SPACE:
        build_path();
        break;
    }

    base_visualizer_client::on_key_down(key);
}

void lib_test_client::build_path()
{
    if (!start_.is_initialized() || !end_.is_initialized())
        return;

    path_.clear();
    r.get_path(*start_, *end_, path_);
}


vertex_id lib_test_client::get_vertex_by_coord(const vis_coord& c) const
{
    vertex_id cand = 0;
    double cand_dist = std::numeric_limits<double>::max();
    for (auto it = pgraph_->v_begin(); it != pgraph_->v_end(); ++it)
    {
        const vertex_id id = pgraph_->get_vertex_id(it);

        const vis_coord d = it->data.c - c;
        const double dist = std::max(fabs(d.x), fabs(d.y));

        if (dist < cand_dist)
        {
            cand_dist = dist;
            cand = id;
        }
    }
    return cand;
}
