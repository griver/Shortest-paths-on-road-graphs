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
    /*auto draw_vertex = [&] (my_graph::vertex_id id) 
    {
        const int frame = 3;
        coord<int> vert_coord = scope.world2screen(pgraph_->get_vertex(id).get_data().c);
        coord<int> eps (frame, frame);
        d.draw_rect(vert_coord - eps, vert_coord + eps);
    };*/

    d.set_bg_color(0, 0, 0);

    d.set_color(64, 64, 64);
    d.draw_buffers(g_desc.vb, 0, pgraph_->v_count(), g_desc.ib, 0, pgraph_->e_count());


}
