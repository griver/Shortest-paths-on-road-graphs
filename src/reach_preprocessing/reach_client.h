#pragma once

#include "../shared/base_visualizer_client.h"

class reach_client : public base_visualizer_client
{
public:
    typedef reach_vertex vertex;
    typedef reach_edge edge;
    typedef reach_graph graph;

public:
    reach_client (const string &filename, visualizer *pvis, draw_scope *pscope);
    ~reach_client();

    /*virtual void on_mouse_move (int x, int y);
    virtual void on_mouse_down (int x, int y, int button);
    virtual void on_mouse_up   (int x, int y, int button);
    virtual void on_key_down   (int key);
    virtual void on_wheel      (int delta);
    virtual void on_resize     (int width, int height);*/

    void on_mouse_move (int x, int y);
    void on_mouse_down (int x, int y, int button);
    void on_key_down   (int key);
    void draw (visualizer &d, draw_scope &scope);

private:
    void print_stats() const;
    void delete_verts();
    void check_multiple_edges();
private:
    bool draw_graph;
    bool draw_shortcuts;

    boost::scoped_ptr<reach_graph> pgraph_;
    graph_desc g_desc;

    boost::optional<my_graph::vertex_id> selected_;
    bool shortcuts_added;
    size_t n_original_edges;

    boost::optional<my_graph::vertex_id> center_;
    coord<float> mouse_coords_world_;
    float radius_;

    boost::optional<tree_desc> lit1_, lit2_;
    unordered_set<my_graph::vertex_id> marked_;
};