#include "stdafx.h"
#include "reach_client.h"

void load_osm(const string &path, vis_graph &ref_graph, vis_coord &ref_mins, vis_coord &ref_maxs);

void add_shortcuts (vis_graph &g, size_t degree);


reach_client::reach_client(const string &filename, visualizer *pvis, draw_scope *pscope)
    : base_visualizer_client(pvis, pscope)
    , pgraph_(new vis_graph())
    , shortcuts_added (false)
    
    , draw_graph (true)
    , draw_shortcuts (false)
{
    vis_coord mins, maxs;

    load_osm(filename, *pgraph_, mins, maxs);

    print_stats();
    n_original_edges = pgraph_->e_count();
    add_shortcuts(*pgraph_, 3);

    g_desc = upload_graph(*pgraph_);
    zoom(mins, maxs);
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

    
    if (draw_shortcuts && pgraph_->e_count() > n_original_edges)
    {
        d.set_color(64, 0, 64);
        d.draw_buffers(g_desc.vb, 0, pgraph_->v_count(), g_desc.ib, n_original_edges, pgraph_->e_count() - n_original_edges);
    }

    if (draw_graph)
    {
        d.set_color(64, 64, 64);
        d.draw_buffers(g_desc.vb, 0, pgraph_->v_count(), g_desc.ib, 0, n_original_edges);
    }

    /*d.set_color(255, 0, 0);
    draw_vertex (483413);
    d.set_color(255, 255, 0);
    draw_vertex (483416);
    draw_vertex (156256);
    draw_vertex (263719);*/

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

void reach_client::on_key_down(int key)
{
    switch (key)    
    {
    case '1':
        draw_graph = !draw_graph;
        break;
    case '2':
        draw_shortcuts = !draw_shortcuts;
        break;
    case 'I':
    case 'i':
        if (selected_.is_initialized())
        {
            const vis_vertex &v = pgraph_->get_vertex(*selected_);
            cout << "Vertex " << *selected_ << ", osm id " << v.get_data().orig_id << endl;
            for (auto it = v.out_begin(); it != v.out_end(); ++it)
            {
                const vis_edge &e = pgraph_->get_edge(it->e);
                cout << "  " << "vertex " << it->v << " edge " << it->e << " len " << e.get_data().len << endl;
            }
        }
        break;
    }
    base_visualizer_client::on_key_down(key);
}

void reach_client::print_stats() const
{
    vector<int> stats;
    for (auto it = pgraph_->v_begin(); it != pgraph_->v_end(); ++it)
    {
        size_t degree = it->get_degree();
        if (degree >= stats.size())
            stats.resize(degree + 1, 0);
        ++stats[degree];
    }

    cout << "Degree stats: " << endl;
    for (size_t i = 0; i < stats.size(); ++i)
        if (stats[i] != 0)
            cout << i << ": " << stats[i] << endl;
}

