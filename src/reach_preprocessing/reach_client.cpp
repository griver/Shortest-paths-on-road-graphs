#include "stdafx.h"
#include "../shared/graph_filter.h"
#include "../shared/osm_loader.h"
#include "penalties_preprocessor.h"
#include "reach_client.h"
#include "reach_dijkstra.h"
#include "c9_astar.h"
#include "shortcuts.h"
#include "../shared/grid.h"



void test_reach_updater(const reach_graph &ref_graph, my_graph::vertex_id start, my_graph::edge_weight dist, my_graph::path_map &ref_out, my_graph::path_map &ref_out2);
void draw_circle(const reach_graph &ref_graph, my_graph::vertex_id start, my_graph::edge_weight dist, my_graph::path_map &ref_out, my_graph::path_map &ref_out2);
void test_circles (const reach_graph &g);
//reach_graph *run_reaches_update(const reach_graph &ref_graph, my_graph::vertex_id start, my_graph::vertex_id end)
reach_graph *run_reaches_update(const reach_graph &ref_graph, my_graph::edge_weight dist);


void inject_ids(reach_graph &g)
{
    for (auto it = g.v_begin(); it != g.v_end(); ++it)
        it->get_data().orig_id = g.get_vertex_id(it);
    
    for (auto it = g.e_begin(); it != g.e_end(); ++it)
        it->get_data().orig_id = g.get_edge_id(it);
}

void load_shortcuts_from_array(reach_graph &g, const vector<shortcut> &shortcuts)
{
    for (auto it = shortcuts.begin(); it != shortcuts.end(); ++it)
    {
        edge_weight len = 0;
        for (auto e_it = it->edges.begin(); e_it != it->edges.end(); ++e_it)
        {
            const reach_edge &e = g.get_edge(*e_it);
            len += e.get_data().len;
        }
        const reach_edge_data data (len);
        g.add_edge(it->verts.front(), it->verts.back(), data);
    }
}

void get_mins_maxs(const reach_graph &g, vis_coord &mins, vis_coord &maxs)
{
    bool init = false;
    for (auto it = g.v_begin(); it != g.v_end(); ++it)
    {
        vis_coord coord = it->get_data().c;

        if (!init)
        {
            mins = coord;
            maxs = coord;
            init = true;
        }
        if (coord.x < mins.x) mins.x = coord.x;
        if (coord.y < mins.y) mins.y = coord.y;
        if (coord.x > maxs.x) maxs.x = coord.x;
        if (coord.y > maxs.y) maxs.y = coord.y;
    }
}

reach_client::reach_client(const string &filename, visualizer *pvis, draw_scope *pscope)
    : base_visualizer_client(pvis, pscope)
    , filename_(filename)
    , pgraph_(new reach_graph())
    , shortcuts_added (false)
    
    , draw_graph (true)
    , draw_shortcuts (false)

{
    const bool save_shortcuts = true, save_graph = false;



    if(save_graph)
    {
        load_osm(filename, *pgraph_, mins, maxs);

        cout << "saving graph...";
        std::ofstream ofs (filename + ".szd", std::ios_base::out | std::ios_base::binary);
        boost::archive::binary_oarchive ar (ofs);
        ar << *pgraph_;
        cout << "done" << endl;
    }
    else
    {
        cout << "loading graph...";
        std::ifstream ifs (filename + ".szd", std::ios_base::in | std::ios_base::binary);
        boost::archive::binary_iarchive ar (ifs);
        ar >> *pgraph_;
        cout << "done" << endl;
        get_mins_maxs(*pgraph_, mins, maxs);
    }

    cout << "Graph: " << pgraph_->v_count() << " verts, " << pgraph_->e_count() << " edges" << endl;
    inject_ids(*pgraph_);


    print_stats();
    n_original_edges = pgraph_->e_count();
    
    /*vector<shortcut> shortcuts;
    
    if (save_shortcuts) 
    {
        add_shortcuts(*pgraph_, 3, shortcuts, pgraph_->e_count());
        cout << shortcuts.size() << " shortcuts in array" << endl;

        cout << "saving shortcuts...";
        std::ofstream ofs (filename + ".shortcuts", std::ios_base::out | std::ios_base::binary);
        boost::archive::binary_oarchive ar (ofs);
        ar << shortcuts;
        cout << "done" << endl;
    } 
    else 
    {
        cout << "loading shortcuts...";
        std::ifstream ofs (filename + ".shortcuts", std::ios_base::in | std::ios_base::binary);
        boost::archive::binary_iarchive ar (ofs);
        ar >> shortcuts;
        load_shortcuts_from_array(*pgraph_, shortcuts);
        cout << "done" << endl;
    }*/

    pprep_.reset(new penalties_preprocessor (pgraph_.get(), 0.01, 2));

    g_desc = upload_graph(*pgraph_);
    zoom(mins, maxs);
    
    square1_ = square2_ = mins;
    selecting_ = false;

    pgrid_.reset(new grid(mins, maxs, 100, 100));

    load_reaches_simple(filename);
    upload_reaches();
    //check_multiple_edges();
}

reach_client::~reach_client()
{
    free_vb(g_desc.vb);
    free_ib(g_desc.ib);
}



void reach_client::load_reaches(const string &filename)
{
    cout << "loading reaches...";
    std::ifstream ifs (filename + ".reaches_shortcuts", std::ios_base::in | std::ios_base::binary);
    const edge_weight big_reach = std::max (maxs.x - mins.x, maxs.y - mins.y);
    if (!ifs.is_open())
    {
        cout << "not found" << endl;

        reaches_.resize(pgraph_->v_count(), big_reach);
        return;
    }
    boost::archive::binary_iarchive ar (ifs);
    ar >> reaches_;
    cout << "done" << endl;
    
    //cout << "uploading reaches" << endl;


}

void reach_client::load_reaches_simple(const string &filename)
{
    cout << "loading simple reaches...";
    std::ifstream ifs (filename + ".c9_reaches", std::ios_base::in | std::ios_base::binary);
    const edge_weight big_reach = std::max (maxs.x - mins.x, maxs.y - mins.y);
    reaches_.resize(pgraph_->v_count(), big_reach);
    if (!ifs.is_open())
    {
        cout << "not found" << endl;
        return;
    }

    for (size_t i = 0; i < reaches_.size(); ++i)
    {
        unsigned int temp;
        ifs.read((char*)&temp, 4);
        ifs.read((char*)&temp, 4);
        ifs.read((char*)&(reaches_[i]), sizeof(edge_weight));

        //ofs.write((char*)&i, sizeof(size_t));
    }
    
}


void reach_client::upload_reaches()
{
    const edge_weight big_reach = std::max (maxs.x - mins.x, maxs.y - mins.y);
    edge_weight min_reach = big_reach;
    edge_weight max_reach = 0;
    for (size_t i = 0; i < reaches_.size(); ++i)
    {
        if (reaches_[i] > max_reach)
            max_reach = reaches_[i];
        if (reaches_[i] < min_reach)
            min_reach = reaches_[i];
    }
    cout << "Max reach: " << max_reach << endl;
    cout << "Min reach: " << min_reach << endl;

    b_vertex *pv = get_visualizer().lock_vb(g_desc.vb, 0, g_desc.vb_size);
    for (size_t i = 0; i < reaches_.size(); ++i)
    {
        double n = (reaches_[i] - min_reach) / (max_reach - min_reach);
        n = pow (n, 1);
        int c = n * 255;
        pv[i].color = (0xff << 24) | (c << 16) | (c << 8) | c;
    }
    get_visualizer().unlock_vb (g_desc.vb);

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

    if (draw_graph && pgraph_->v_count() != 0 && pgraph_->e_count() != 0)
    {
        //d.set_color(64, 64, 64);
        d.unset_color();
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
    /*d.set_color(255, 0, 255);
    draw_vertex (g_marked_v1);
    draw_vertex (g_marked_v2);*/
    d.set_color(255, 0, 255);
    for (auto it = pgraph_->v_begin(); it != pgraph_->v_end(); ++it)
    {
        const vertex_id id = pgraph_->get_vertex_id(it);
        if (it->data.orig_id == 77722)
        {
            draw_vertex(id);
        }
    }
    if (selected_.is_initialized())
    {
        d.set_color(0, 255, 0);
        draw_vertex(*selected_);
    }


    d.set_color(0, 255, 255);
    d.draw_rect_world(square1_, square2_);


}

void reach_client::on_mouse_down(int x, int y, int button)
{
    boost::optional<my_graph::vertex_id> hover = get_vertex(*pgraph_, coord<int>(x, y));
    if (button == 0)
        selected_ = hover;

    /*if (button == 1 && hover.is_initialized()) {
        if (marked_.count(*hover) == 0)
            marked_.insert(*hover);
        else
            marked_.erase(*hover);
    }*/
    if (button == 1 && selected_.is_initialized()&& hover.is_initialized())
    {
        run_astar(*selected_, *hover);
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
            cout << "Vertex " << *selected_ << ", orig id " << v.get_data().orig_id << endl;
            cout << "Reach " << pprep_->get_reaches()[v.get_data().orig_id] << endl;
            cout << "Penalty " << v.data.penalty << endl;
            for (auto it = v.out_begin(); it != v.out_end(); ++it)
            {
                const reach_edge &e = pgraph_->get_edge(it->e);
                cout << "  " << "vertex " << it->v << " edge " << it->e << " orig id " << e.get_data().orig_id << " len " << e.get_data().len << endl;
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
        reset_graph(pprep_->iterate());
        if (pgraph_->v_count() == 0)
            pprep_->save_reaches_and_shortcurs(filename_);
        break;
        //pgraph_.reset(pprep_->iterate())
        /*if (lit1_.is_initialized())
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
        break;*/
    case 'C':
    case 'c':
        selecting_ = !selecting_;
        if (selecting_)
        {
            square1_ = mouse_coords_world_;
            square2_ = mouse_coords_world_;
        }
        cout << "Mouse: " << mouse_coords_world_ << endl;
        break;
    case 'O':
    case 'o':
        run_reaches_update(*pgraph_, 0.02);
        break;
    case 's':
    case 'S':
        ::add_shortcuts_temp(*pgraph_);
        reset_graph(pgraph_.get());
        break;
    case '9':
        if (selected_.is_initialized())
            build_c9_tree(*selected_);
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

    if (selecting_)
        square2_ = mouse_coords_world_;

    base_visualizer_client::on_mouse_move(x, y);
}

void reach_client::delete_verts()
{

    double x1 = std::min(square1_.x, square2_.x);
    double y1 = std::min(square1_.y, square2_.y);
    double x2 = std::max(square1_.x, square2_.x);
    double y2 = std::max(square1_.y, square2_.y);


    reach_graph *dst = new reach_graph();

    my_graph::graph_filter<reach_vertex_data, reach_edge_data> filter;
    filter.set_verts_filter([&](my_graph::vertex_id id) -> bool 
    {
        const reach_vertex& v = pgraph_->get_vertex(id);
        const vis_coord &c = v.get_data().c;
        return (c.x >= x1 && c.y >= y1 && c.x <= x2 && c.y <= y2);
    });

    filter.filter_graph(*pgraph_, *dst);
    
    pgraph_.reset(dst);
    free_vb(g_desc.vb);
    free_ib(g_desc.ib);

    g_desc = upload_graph(*pgraph_);

    cout << "saving graph...";
    std::ofstream ofs ("cropped.szd", std::ios_base::out | std::ios_base::binary);
    boost::archive::binary_oarchive ar (ofs);
    ar << *pgraph_;
    cout << "done" << endl;


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

void reach_client::reset_graph(reach_graph *p)
{
    if (pgraph_.get() != p)
    {
        selected_.reset();
        pgraph_.reset(p);
    }

    free_vb(g_desc.vb);
    free_ib(g_desc.ib);
    if (pgraph_->v_count() != 0 && pgraph_->e_count() != 0)
        g_desc = upload_graph(*pgraph_);

}

void reach_client::build_c9_tree(vertex_id root_id)
{
    tree_.clear();
    const vertex &root = pgraph_->get_vertex(root_id);
    const coord<int> root_cell = pgrid_->getCell(root.data.c);

    DWORD time = timeGetTime();
    
    reach_dijkstra d (*pgraph_, root_id, tree_, [&](vertex_id id) -> bool 
    {
        const vertex &v = pgraph_->get_vertex(id);
        const coord<int> d = pgrid_->getCell(v.data.c) - root_cell;
        return std::max(std::abs(d.x), std::abs(d.y)) <= 4;
    });

    size_t count = 0;
    while (!d.done())
    {
        d.iterate();
        ++count;
    }

    time = timeGetTime() - time;

    cout << "c9 time " << time << ": " << count << " verts" << endl;

    tree_desc desc;
    desc.ib = g_desc.ib;
    desc.ib_size = g_desc.ib_size;
    lit1_.reset(upload_tree(tree_, desc));
}

void reach_client::run_astar(vertex_id id1, vertex_id id2)
{
    const int C9_RADIUS = 4;
    
    const vertex &v1 = pgraph_->get_vertex(id1);
    const vertex &v2 = pgraph_->get_vertex(id2);
    const coord<int> v1_cell = pgrid_->getCell(v1.data.c);
    const coord<int> v2_cell = pgrid_->getCell(v2.data.c);
    
    const coord<int> d = v2_cell - v1_cell;
    const int dist = std::max(std::abs(d.x), std::abs(d.y));
    if (dist > 4)
    {
        cout << "Too far for c9 astar!" << endl;
        return;
    }

    tree_.clear();
    c9_astar astar (*pgraph_, id1, id2, tree_, *pgrid_, reaches_);
    /*{
        const vertex &v = pgraph_->get_vertex(id);

        

        const coord<int> cell_d = pgrid_->getCell(v.data.c) - v1_cell;
        return std::max(std::abs(cell_d.x), std::abs(cell_d.y)) <= C9_RADIUS;
    });*/
    while (!astar.done())
        astar.iterate();

    if (!tree_.empty())
    {
        tree_desc desc;
        desc.ib = g_desc.ib;
        desc.ib_size = g_desc.ib_size;
        lit1_.reset(upload_tree(tree_, desc));
    }
}
