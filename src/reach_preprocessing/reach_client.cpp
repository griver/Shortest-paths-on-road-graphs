#include "stdafx.h"
#include "../shared/graph_filter.h"
#include "../shared/osm_loader.h"
#include "reach_client.h"
#include "reach_dijkstra.h"
#include "shortcuts.h"


void test_reach_updater(const reach_graph &ref_graph, my_graph::vertex_id start, my_graph::edge_weight dist, my_graph::path_map &ref_out, my_graph::path_map &ref_out2);
void draw_circle(const reach_graph &ref_graph, my_graph::vertex_id start, my_graph::edge_weight dist, my_graph::path_map &ref_out, my_graph::path_map &ref_out2);
void test_circles (const reach_graph &g);
//reach_graph *run_reaches_update(const reach_graph &ref_graph, my_graph::vertex_id start, my_graph::vertex_id end)
reach_graph *run_reaches_update(const reach_graph &ref_graph, my_graph::edge_weight dist);


/*void inject_edge_ids(reach_graph &g)
{
    for (auto it = g.v_begin(); it != g.v_end(); ++g)
    {
        const vertex_id id = g.get_vertex_id(it);
        it->get_data().
    }
}*/

reach_client::reach_client(const string &filename, visualizer *pvis, draw_scope *pscope)
    : base_visualizer_client(pvis, pscope)
    , pgraph_(new reach_graph())
    , shortcuts_added (false)
    
    , draw_graph (true)
    , draw_shortcuts (false)
{
    reach_coord mins, maxs;

    load_osm(filename, *pgraph_, mins, maxs);

    print_stats();
    n_original_edges = pgraph_->e_count();
    
    vector<shortcut> shortcuts;
    add_shortcuts(*pgraph_, 3, shortcuts, pgraph_->e_count());
    cout << shortcuts.size() << " shortcuts in array" << endl;

    g_desc = upload_graph(*pgraph_);
    zoom(mins, maxs);

    check_multiple_edges();
}

reach_client::~reach_client()
{
    free_vb(g_desc.vb);
    free_ib(g_desc.ib);
}

extern vertex_id g_marked_v1, g_marked_v2;

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

    
    /*if (draw_shortcuts && pgraph_->e_count() > n_original_edges)
    {
        d.set_color(64, 0, 64);
        d.draw_buffers(g_desc.vb, 0, pgraph_->v_count(), g_desc.ib, n_original_edges, pgraph_->e_count() - n_original_edges);
    }*/

    if (draw_graph)
    {
        d.set_color(64, 64, 64);
        d.draw_buffers(g_desc.vb, 0, pgraph_->v_count(), g_desc.ib, 0, pgraph_->e_count());
    }


    /*d.set_color(255, 0, 0);
    draw_vertex (483413);
    d.set_color(255, 255, 0);
    draw_vertex (483416);
    draw_vertex (156256);
    draw_vertex (263719);

    d.set_color(255, 0, 0);
    draw_vertex(1103066);
    draw_vertex(266927);*/



    if (center_.is_initialized())
    {
        const coord<float> c = pgraph_->get_vertex(*center_).get_data().c;
        const coord<float> r (radius_, radius_);
        d.set_color(0, 255, 0);
        d.draw_rect_world(c - r, c + r);
    }

    if (lit1_.is_initialized())
    {
        d.set_color(128, 0, 0);
        d.draw_buffers(g_desc.vb, 0, g_desc.vb_size, lit1_->ib, 0, lit1_->ib_size);    
    }
    if (lit2_.is_initialized())
    {
        d.set_color(0, 0, 128);
        d.draw_buffers(g_desc.vb, 0, g_desc.vb_size, lit2_->ib, 0, lit2_->ib_size);    
    }

    d.set_color(255, 255, 0);
    for (auto it = marked_.begin(); it != marked_.end(); ++it) {
        draw_vertex (*it);
    }
    if (selected_.is_initialized())
    {
        d.set_color(0, 255, 0);
        draw_vertex(*selected_);
    }
    d.set_color(255, 0, 255);
    draw_vertex (g_marked_v1);
    draw_vertex (g_marked_v2);


}

void reach_client::on_mouse_down(int x, int y, int button)
{
    boost::optional<my_graph::vertex_id> hover = get_vertex(*pgraph_, coord<int>(x, y));
    if (button == 0)
        selected_ = hover;

    if (button == 1 && hover.is_initialized()) {
        if (marked_.count(*hover) == 0)
            marked_.insert(*hover);
        else
            marked_.erase(*hover);
    }


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
            const reach_vertex &v = pgraph_->get_vertex(*selected_);
            cout << "Vertex " << *selected_ << /*", osm id " << v.get_data().osm_id <<*/ endl;
            for (auto it = v.out_begin(); it != v.out_end(); ++it)
            {
                const reach_edge &e = pgraph_->get_edge(it->e);
                cout << "  " << "vertex " << it->v << " edge " << it->e << " len " << e.get_data().len << endl;
            }
        }
        break;

    /*case 'L':
    case 'l':
        if (lit_.is_initialized())
        {
            get_visualizer().free_ib(lit_->ib);
            lit_.reset();
        }
        
        if (selected_.is_initialized())
        {
            my_graph::path_map m;
            reach_dijkstra d (*pgraph_, *selected_, m);
            while (!d.done())
                d.iterate();

            tree_desc desc;
            desc.ib = g_desc.ib;
            desc.ib_size = g_desc.ib_size;

            lit_.reset(upload_tree(m, desc));
        }
        break;*/

    case 'M':
    case 'm':
        if (lit1_.is_initialized())
        {
            get_visualizer().free_ib(lit1_->ib);
            lit1_.reset();
        }
        if (lit2_.is_initialized())
        {
            get_visualizer().free_ib(lit2_->ib);
            lit2_.reset();
        }
        if (selected_.is_initialized())
        {
            my_graph::path_map m1, m2;

            draw_circle (*pgraph_, *selected_, 0.25, m1, m2);
            
            tree_desc desc;
            desc.ib = g_desc.ib;
            desc.ib_size = g_desc.ib_size;

            lit1_.reset(upload_tree(m1, desc));
            lit2_.reset(upload_tree(m2, desc));
            cout << "Tree size: " << m1.size() << endl;
        }
        break;
    case 'N':
    case 'n':
        test_circles (*pgraph_);
        break;
    case 'O':
    case 'o':
        run_reaches_update(*pgraph_, 0.02);
        break;
    case VK_DELETE:
        delete_verts();
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


void reach_client::on_mouse_move(int x, int y)
{
    mouse_coords_world_ = get_scope().screen2world(coord<int>(x, y));
    base_visualizer_client::on_mouse_move(x, y);
}

void reach_client::delete_verts()
{
    if (marked_.empty())
        return;

    reach_graph *dst = new reach_graph();

    my_graph::graph_filter<reach_vertex_data, reach_edge_data> filter;
    filter.set_verts_filter([&](my_graph::vertex_id id) -> bool 
    {
        return marked_.count(id) == 0;
    });

    filter.filter_graph(*pgraph_, *dst);


    pgraph_.reset(dst);
    free_vb(g_desc.vb);
    free_ib(g_desc.ib);

    g_desc = upload_graph(*pgraph_);

    marked_.clear();
    selected_.reset();
}

void reach_client::check_multiple_edges()
{
    size_t counter = 0;
    for (auto it = pgraph_->v_begin(); it != pgraph_->v_end(); ++it)
    {
        const vertex_id id = pgraph_->get_vertex_id(it);
        const vertex& v = *it;
        unordered_set<vertex_id> adj_set;
        
        for (auto adj_it = v.out_begin(); adj_it != v.out_end(); ++adj_it)
        {
            if (adj_set.count(adj_it->v) != 0)
            {
                //cout << "MULTIPLE PAIR: " << id << " - " << adj_it->v << endl;
                ++counter;
            }
            adj_set.insert(adj_it->v);
        }
    }

    cout << "Total multiple edges: " << counter << endl;
}
