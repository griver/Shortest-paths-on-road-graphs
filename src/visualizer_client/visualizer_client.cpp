#include "stdafx.h"
#include "../shared/client.h"
#include "../shared/visualizer.h"
#include "visualizer_client.h"
#include "../shared/common_algorithms/dijkstra.h"

namespace my_graph
{
    template <>
    inline edge_weight dijkstra_class<vis_vertex_data, vis_edge_data>::get_weight(const edge &e)
    {
        static bool noticed = false;
        if (!noticed)
        {
            cout << "Using template weight!" << endl;
            noticed = true;
        }
        return e.get_data().len_;
    }
}

void run_vis_dijkstra (const vis_graph &g, my_graph::vertex_id start, my_graph::vertex_id end, my_graph::path_map *pout1, my_graph::path_map *pout2, my_graph::path_map *ppath)
{
    /*my_graph::dijkstra_class<vis_vertex_data, vis_edge_data> d (g, NULL, *pout1);
    d.init (start);
    while (pout1->count(end) == 0 && !d.done())
        d.iterate();

    if (pout1->count(end) == 0)
        return;

    my_graph::path_vertex pv = pout1->find(end)->second;
    while (pv.parent.is_initialized())
    {
        (*ppath)[pv.id] = pv;
        pv = pout1->find(*pv.parent)->second;
    }*/

    run_dijkstra(get_vis_weight, g, start, end, pout1, pout2, ppath);
}

void load_osm(const string &path, vis_graph *dst, coord<double> &ref_mins, coord<double> &ref_maxs);
    
coord<double> g_mins, g_maxs;

visualizer_client::visualizer_client(const graph_loader& loader, visualizer *pvis, draw_scope *pscope)
:   dragging_(false)
,   last_coord_(0,0)
,   pd_(pvis)
,   pscope_(pscope)

{
    /*coord<long> mins, maxs;

    my_graph::load_graph (filename + ".co", g);
    mins = my_graph::g_loader_mins;
    maxs = my_graph::g_loader_maxs;
    my_graph::load_graph (filename + ".gr", g);
    my_graph::load_graph (filename + ".re", g);

    std::cout << "Mins: " << mins << "\n";
    std::cout << "Maxs: " << maxs << "\n";*/


    loader(g, mins, maxs);
    //load_osm (filename, &g, mins, maxs);
    cout << "Mins: " << mins << endl;
    cout << "Maxs: " << maxs << endl;

    ::g_mins = mins;
    ::g_maxs = maxs;

    float scale = std::max(maxs.x - mins.x, maxs.y - mins.y);
    pscope_->reset_scope(mins, 1000.0f / scale);

    vb = pd_->create_vb(g.v_count());
    ib = pd_->create_ib(g.e_count());

    ib_lit1 = pd_->create_ib(g.e_count());
    ib_lit2 = pd_->create_ib(g.e_count());
    ib_path = pd_->create_ib(g.e_count());

    build_graph();

    pd_->set_client(this);

    register_algorithm("Dijkstra", run_vis_dijkstra);

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

void visualizer_client::build_graph()
{
    size_t index;

    b_vertex *pv = pd_->lock_vb(vb, 0, g.v_count());

    index = 0;
    for (vis_graph::v_iterator it = g.v_begin(); it != g.v_end(); ++it, ++index)
    {
        vis_vertex_data& data = it->second.get_data();

        pv[index].x = data.c.x;
        pv[index].y = data.c.y;
        pv[index].z = 0;

        int color = 0xFFFF0000;//std::min(static_cast<int>((static_cast<double>(data.reach) / reach_limiter) * 1000.0), 255);

        //pv[index].color = D3DCOLOR_XRGB(color, 128-color/2, 128-color/2);

        data.buffer_index_ = index;
    }

    pd_->unlock_vb (vb);

    b_edge* pe = pd_->lock_ib(ib, 0, g.e_count());


    index = 0;
    for (vis_graph::e_iterator it = g.e_begin(); it != g.e_end(); ++it, ++index)
    {
        vis_edge_data& data = it->second.get_data();

        pe[index].v1 = it->second.get_v1().get_data().buffer_index_;
        pe[index].v2 = it->second.get_v2().get_data().buffer_index_;

        data.buffer_index_ = index;
    }

    pd_->unlock_ib (ib);
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
            const vis_vertex_data &data = g.get_vertex(*selected_).get_data();
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
            cout << " at " << g.get_vertex(*selected_).get_data().c << endl;
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
    pd_->set_bg_color(0, 0, 64);
    pd_->draw_begin();

    pd_->set_color(64, 64, 64);
    pd_->draw_buffers(vb, g.v_count(), ib, g.e_count());

    if (!lit1_.empty())
    {
        pd_->set_color (128, 0, 0);
        pd_->draw_buffers(vb, g.v_count(), ib_lit1, lit1_.size());
    }

    if (!lit2_.empty())
    {
        pd_->set_color (0, 0, 128);
        pd_->draw_buffers(vb, g.v_count(), ib_lit2, lit2_.size());
    }

    if (!path_.empty())
    {
        pd_->set_color (255, 255, 255);
        pd_->draw_buffers(vb, g.v_count(), ib_path, path_.size());
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
    for (vis_graph::v_const_iterator it = g.v_begin(); it != g.v_end(); ++it )
    {
        coord<int> d = pscope_->world2screen(it->second.get_data().c);
        if (std::max (abs (d.x - c.x), abs (d.y - c.y)) <= 3)
        {
            hover_.reset (it->first);
            return;
        }

    }
    hover_.reset();
}


void visualizer_client::draw_vertex( my_graph::vertex_id id, int frame, const std::string& str )
{
    coord<int> vert_coord = pscope_->world2screen(g.get_vertex(id).get_data().c);
    coord<int> eps (frame, frame);
    pd_->draw_rect(vert_coord - eps, vert_coord + eps);
    if (!str.empty())
        pd_->draw_text(vert_coord, str);
}


void visualizer_client::register_algorithm(const string &name, const algo_fn &fn)
{
    path_algorithm algo = {name, fn};
    algorithms_.push_back (algo);
}

void visualizer_client::run_algorithm (const path_algorithm &algorithm)
{
    lit1_.clear();
    lit2_.clear();
    path_.clear();

    DWORD time = timeGetTime();
    algorithm.fn(g, *start_, *end_, &lit1_, &lit2_, &path_);
    time = timeGetTime() - time;
    
    cout << endl << algorithm.name << " ran for " << time << " ms." << endl;
    cout << lit1_.size() + lit2_.size() << " verts visited." << endl;

    update_lit();
}


void visualizer_client::update_path_map(const my_graph::path_map &m, ib_id ib_dst, b_edge* pe_src)
{

    b_edge* pe_dst = pd_->lock_ib(ib_dst, 0, g.e_count());

    size_t index = 0;
    for (my_graph::path_map::const_iterator it = m.begin(); it != m.end (); ++it, ++index)
    {
        if (!it->second.inc)
            pe_dst[index] = pe_src[0];
        else
            pe_dst[index] = pe_src[g.get_edge(*(it->second.inc)).get_data().buffer_index_];
    }

    pd_->unlock_ib(ib_dst);


}

void visualizer_client::update_lit()
{
    b_edge* pe_src = pd_->lock_ib(ib, 0, g.e_count(), 0);

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
    const vis_vertex &v = g.get_vertex(id);
    for (vis_vertex::ve_const_iterator it = v.out_begin(); it != v.out_end(); ++it)
    {
        const vis_edge &e = *(*it);
        my_graph::vertex_id id1 = e.get_v2().get_id();
        my_graph::path_vertex pv(id1, get_vis_weight(e), e.get_id(), id);
        lit1_.insert(my_graph::path_map::value_type(id1, pv));
    }
    cout << lit1_.size() << " verts marked" << endl;
    update_lit();
}
