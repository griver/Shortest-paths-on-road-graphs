#include "stdafx.h"
#include "reach_client.h"

void load_osm(const string &path, vis_graph &ref_graph, vis_coord &ref_mins, vis_coord &ref_maxs);

reach_client::reach_client(const string &filename, visualizer *pvis, draw_scope *pscope)
    : base_visualizer_client(pvis, pscope)
    , pgraph_(new vis_graph())
{
    vis_coord mins, maxs;

    load_osm(filename, *pgraph_, mins, maxs);
    zoom(mins, maxs);

    g_desc = upload_graph(*pgraph_);
}

reach_client::~reach_client()
{
    free_vb(g_desc.vb);
    free_ib(g_desc.ib);
}


void reach_client::draw(visualizer &d, draw_scope &scope)
{
    auto draw_vertex = [&] (my_graph::vertex_id id) 
    {
        const int frame = 3;
        coord<int> vert_coord = scope.world2screen(pgraph_->get_vertex(id).get_data().c);
        coord<int> eps (frame, frame);
        d.draw_rect(vert_coord - eps, vert_coord + eps);
    };
    
    d.set_bg_color(0, 0, 0);

    d.set_color(32, 32, 32);
    d.draw_buffers(g_desc.vb, pgraph_->v_count(), g_desc.ib, pgraph_->e_count());

    if (selected_.is_initialized())
    {
        d.set_color(0, 255, 0);
        draw_vertex(*selected_);
    }
}

void reach_client::on_mouse_down(int x, int y, int button)
{
    selected_ = get_vertex(*pgraph_, coord<int>(x, y));
    base_visualizer_client::on_mouse_down(x, y, button);
}

