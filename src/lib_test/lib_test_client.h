#pragma once

#include "../shared/base_visualizer_client.h"
#include "../shared/new_vis_graph.h"
#include "../routes_lib/routes.h"

class lib_test_client : public base_visualizer_client
{
public:
    typedef vis_vertex vertex;
    typedef vis_edge edge;
    typedef vis_graph graph;

public:
    lib_test_client (const string &filename, visualizer *pvis, draw_scope *pscope);
    ~lib_test_client();

    void draw (visualizer &d, draw_scope &scope);
    virtual void on_mouse_down (int x, int y, int button);
    virtual void on_key_down   (int key);

private:
    void build_path();
    vertex_id get_vertex_by_coord(const vis_coord& c) const;
private:
    routes r;
    const vis_graph *pgraph_;
    graph_desc g_desc;

    optional<vertex_id> selected_;
    optional<vertex_id> start_, end_;
    vector<vis_coord> path_;
};