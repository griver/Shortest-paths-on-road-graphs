#include "stdafx.h"
#include "../shared/client.h"
#include "../shared/visualizer.h"
#include "visualizer_client.h"
//#include "../shared/common_algorithms/dijkstra.h"



void load_osm(const string &path, vis_graph *dst, coord<double> &ref_mins, coord<double> &ref_maxs);
    
coord<double> g_mins, g_maxs;

visualizer_client::visualizer_client (const vis_graph &g, visualizer *pvis, draw_scope *pscope, const vis_coord &mins, const vis_coord &maxs)
:   dragging_(false)
,   last_coord_(0,0)
,   pd_(pvis)
,   pscope_(pscope)
,   pgraph_(NULL)
,   mins_(mins)
,   maxs_(maxs)
,   buffers_initialized(false)
,   vb_size(0)
,   ib_size(0)

{
    
    /*coord<long> mins, maxs;

    my_graph::load_graph (filename + ".co", g);
    mins = my_graph::g_loader_mins;
    maxs = my_graph::g_loader_maxs;
    my_graph::load_graph (filename + ".gr", g);
    my_graph::load_graph (filename + ".re", g);

    std::cout << "Mins: " << mins << "\n";
    std::cout << "Maxs: " << maxs << "\n";*/



    //loader(*pgraph_, mins, maxs);
    //load_osm (filename, &g, mins, maxs);
    cout << "Mins: " << mins << endl;
    cout << "Maxs: " << maxs << endl;

    //::g_mins = mins;
    //::g_maxs = maxs;



    float scale = std::max(maxs.x - mins.x, maxs.y - mins.y);
    vis_coord corner (maxs.x, mins.y);
    pscope_->reset_scope(corner, 1000.0f / scale);

    load_graph(g);

    pd_->set_client(this);

    //register_algorithm("Dijkstra", run_vis_dijkstra);

    algorithm_it_ = algorithms_.begin();
}

visualizer_client::~visualizer_client()
{
    std::cout << "Deleting client...\n";
    pd_->free_vb (vb);
    pd_->free_ib (ib);
    pd_->free_ib (ib_lit1);
    pd_->free_ib (ib_lit2);
    pd_->free_ib (ib_path);
}

void visualizer_client::load_graph(const vis_graph &g)
{
    pgraph_ = &g;

    if (g.v_count() > vb_size)
    {
        if (buffers_initialized)
            pd_->free_vb(vb);

        vb_size = pgraph_->v_count();
        vb = pd_->create_vb(vb_size);
    }

    if (g.e_count() > ib_size)
    {
        if (buffers_initialized)
        {
            pd_->free_ib (ib);
            pd_->free_ib (ib_lit1);
            pd_->free_ib (ib_lit2);
            pd_->free_ib (ib_path);
        }
        ib_size = pgraph_->e_count();
        ib = pd_->create_ib(ib_size);
        ib_lit1 = pd_->create_ib(ib_size);
        ib_lit2 = pd_->create_ib(ib_size);
        ib_path = pd_->create_ib(ib_size);
    }

    buffers_initialized = true;
    lit1_.clear();
    lit2_.clear();
    path_.clear();
    start_ = boost::optional<my_graph::vertex_id>();
    end_ = boost::optional<my_graph::vertex_id>();
    build_graph();
}

void visualizer_client::build_graph()
{
    size_t vertex_index;

    b_vertex *pv = pd_->lock_vb(vb, 0, pgraph_->v_count());
    b_edge *pe = pd_->lock_ib(ib, 0, pgraph_->e_count());
    
    vertex_index = 0;
    for (vis_graph::v_const_iterator it = pgraph_->v_begin(); it != pgraph_->v_end(); ++it, ++vertex_index)
    {
        const vis_vertex &v = *it;
        const vis_vertex_data& data = v.data;

        pv[vertex_index].x = data.c.x;
        pv[vertex_index].y = data.c.y;
        pv[vertex_index].z = 0;

        for (vis_vertex::adj_iterator e_it = v.out_begin(); e_it != v.out_end(); ++e_it)
        {
            size_t edge_index = (*e_it).e;
            pe[edge_index].v1 = vertex_index;
            pe[edge_index].v2 = (*e_it).v;
        }
    }

    pd_->unlock_ib (ib);
    pd_->unlock_vb (vb);
}

void visualizer_client::on_mouse_move(int x, int y)
{
    coord<int> screen (x, y);
    if (dragging_)
    {
        pscope_->drag (screen - last_coord_);
        on_paint();
    }
    last_coord_ = screen;
}

void visualizer_client::on_mouse_down (int x, int y, int button)
{
    coord<int> screen (x, y);
    if (button == 0)
    {
        test_hover (screen);
        selected_ = hover_;
        /*if (selected_)
        {
            const vis_vertex_data &data = g->get_vertex(*selected_).get_data();
            std::cout << "Origin: " << data.orig << ", reach: " << data.reach << "\n";
        }*/
        dragging_ = true;
        last_coord_ = screen;
    }
    else if (button == 1)
    {
    }
    on_paint();
}

void visualizer_client::on_mouse_up (int x, int y, int button)
{
    coord<int> screen (x, y);
    dragging_ = false;
    last_coord_ = screen;
    on_paint();
}

void visualizer_client::on_key_down(int key)
{
    const float ZOOM = 1.2f;
    switch (key)
    {
    case VK_PRIOR:
        pscope_->zoom(ZOOM, last_coord_);
        break;
    case VK_NEXT:
        pscope_->zoom(1.0f / ZOOM, last_coord_);
        break;
    case '1':
        start_ = selected_;
        clear_lit();
        break;
    case '2':
        end_ = selected_;
        clear_lit();
        break;
    case VK_TAB:
        if (!algorithms_.empty ())
        {
            ++algorithm_it_;
            if (algorithm_it_ == algorithms_.end())
                algorithm_it_ = algorithms_.begin();
        }
        clear_lit();
        break;
    case VK_SPACE:
        if (!algorithms_.empty() && start_.is_initialized() && end_.is_initialized())
            run_algorithm(*algorithm_it_);
        break;
    case 'I':
        if (selected_.is_initialized())
        {
            cout << "Vertex id " << (*selected_);
            std::streamsize prec = cout.precision();
            cout.precision(10);
            cout << " at " << pgraph_->get_vertex(*selected_).get_data().c << endl;
            cout.precision(prec);
        }
        break;
    case 'M':
        if (selected_.is_initialized())
            mark_vertex (*selected_);
        break;
    }
    on_paint();
}
void visualizer_client::on_wheel (int delta)
{
    pscope_->zoom (pow(1.2f, delta / 120.0f), last_coord_);
    on_paint();
}

void visualizer_client::on_resize(int width, int height)
{
    //pd_->resize(width, height);
    on_paint();
}

void visualizer_client::on_paint()
{
    pd_->set_bg_color(0, 0, 0);
    pd_->draw_begin();

    pd_->set_color(32, 32, 32);
    pd_->draw_buffers(vb, pgraph_->v_count(), ib, pgraph_->e_count());

    if (!lit1_.empty())
    {
        pd_->set_color (128, 0, 0);
        pd_->draw_buffers(vb, pgraph_->v_count(), ib_lit1, lit1_.size());
    }

    if (!lit2_.empty())
    {
        pd_->set_color (0, 0, 128);
        pd_->draw_buffers(vb, pgraph_->v_count(), ib_lit2, lit2_.size());
    }

    if (!path_.empty())
    {
        pd_->set_color (255, 255, 255);
        pd_->draw_buffers(vb, pgraph_->v_count(), ib_path, path_.size());
    }

    if (selected_)
    {
        stringstream ss;
        my_graph::path_map::const_iterator it = lit1_.find(*selected_);
        if (it != lit1_.end())
        {
            ss << "Dist: " << it->second.d;
        }

        pd_->set_color(0, 255, 0);
        draw_vertex(*selected_, 3, ss.str());
    }

    if (start_)
    {
        pd_->set_color(255, 255, 0);
        draw_vertex(*start_, 2, "start");
    }
    if (end_)
    {
        pd_->set_color(255, 255, 0);
        draw_vertex(*end_, 2, "end");
    }

    if (!algorithms_.empty())
    {
        stringstream ss;
        ss << "Algorithm: " << (*algorithm_it_).name;

        pd_->set_color(255, 255, 255);
        pd_->draw_text(coord<int>(0, 0), ss.str());
    }

    pd_->draw_end();
}

void visualizer_client::test_hover( coord<int> c )
{
    // FIXME: ugly hack, works only for vector!
    my_graph::vertex_id id = 0;
    for (vis_graph::v_const_iterator it = pgraph_->v_begin(); it != pgraph_->v_end(); ++it, ++id)
    {
        coord<int> d = pscope_->world2screen((*it).get_data().c);
        if (std::max (abs (d.x - c.x), abs (d.y - c.y)) <= 3)
        {
            cout << "Orig. id: " << (*it).get_data().orig_id << endl;
            hover_.reset (id);
            return;
        }
    }
    //hover_.reset();
}


void visualizer_client::draw_vertex( my_graph::vertex_id id, int frame, const std::string& str )
{
    coord<int> vert_coord = pscope_->world2screen(pgraph_->get_vertex(id).get_data().c);
    coord<int> eps (frame, frame);
    pd_->draw_rect(vert_coord - eps, vert_coord + eps);
    if (!str.empty())
        pd_->draw_text(vert_coord, str);
}


void visualizer_client::register_algorithm(const string &name, const algo_fn &fn)
{
    path_algorithm algo = {name, fn};
    algorithms_.push_back (algo);
    if (algorithms_.size() == 1)
        algorithm_it_ = algorithms_.begin();

}

void visualizer_client::run_algorithm (const path_algorithm &algorithm)
{
    lit1_.clear();
    lit2_.clear();
    path_.clear();

    DWORD time = timeGetTime();
    algorithm.fn(*pgraph_, *start_, *end_, &lit1_, &lit2_, &path_);
    time = timeGetTime() - time;
    
    cout << endl << algorithm.name << " ran for " << time << " ms." << endl;
    cout << lit1_.size() + lit2_.size() << " verts visited." << endl;

    update_lit();
}


void visualizer_client::update_path_map(const my_graph::path_map &m, ib_id ib_dst, b_edge* pe_src)
{

    b_edge* pe_dst = pd_->lock_ib(ib_dst, 0, pgraph_->e_count());

    size_t index = 0;
    for (my_graph::path_map::const_iterator it = m.begin(); it != m.end (); ++it)
    {
        my_graph::edge_id inc_id = it->second.inc.is_initialized() ? *(it->second.inc) : 0;
                
        pe_dst[index] = pe_src[inc_id];
        ++index;
    }

    pd_->unlock_ib(ib_dst);


}

void visualizer_client::update_lit()
{
    b_edge* pe_src = pd_->lock_ib(ib, 0, pgraph_->e_count(), 0);

    update_path_map(lit1_, ib_lit1, pe_src);
    update_path_map(lit2_, ib_lit2, pe_src);
    update_path_map(path_, ib_path, pe_src);

    pd_->unlock_ib(ib);
}

void visualizer_client::clear_lit()
{
    lit1_.clear();
    lit2_.clear();
    path_.clear();
}

void visualizer_client::mark_vertex(my_graph::vertex_id id)
{
    clear_lit();
    
    const vis_vertex &v = pgraph_->get_vertex(id);
    for (vis_vertex::adj_iterator it = v.out_begin(); it != v.out_end(); ++it)
    {
        my_graph::vertex_id id1 = (*it).v;
        const vis_edge &e = pgraph_->get_edge((*it).e);
        my_graph::path_vertex pv(id1, e.data.len, (*it).e, id);
        lit1_.insert(my_graph::path_map::value_type(id1, pv));
    }
    cout << lit1_.size() << " verts marked" << endl;
    update_lit();
}
