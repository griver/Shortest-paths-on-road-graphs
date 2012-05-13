#pragma once

#include "../shared/base_visualizer_client.h"

struct reach_client : base_visualizer_client
{
    reach_client (const string &filename, visualizer *pvis, draw_scope *pscope);
    ~reach_client();

    /*virtual void on_mouse_move (int x, int y);
    virtual void on_mouse_down (int x, int y, int button);
    virtual void on_mouse_up   (int x, int y, int button);
    virtual void on_key_down   (int key);
    virtual void on_wheel      (int delta);
    virtual void on_resize     (int width, int height);*/

    virtual void on_mouse_down (int x, int y, int button);
    virtual void draw (visualizer &d, draw_scope &scope);

private:
    boost::scoped_ptr<vis_graph> pgraph_;
    graph_desc g_desc;

    boost::optional<my_graph::vertex_id> selected_;
};