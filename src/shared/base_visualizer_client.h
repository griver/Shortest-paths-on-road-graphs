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

    boost::optional<my_graph::vertex_id> get_vertex (const vis_graph &g, const coord<int> &coord);

    graph_desc upload_graph (const vis_graph &g);
    tree_desc upload_tree (const my_graph::path_map &m, ib_id src_id);

    void free_vb (vb_id); 
    void free_ib (ib_id);
private:
    visualizer *pd_;
    draw_scope *pscope_;
    bool dragging_;
    coord<int> last_coord_;
};