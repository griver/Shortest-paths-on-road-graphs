#pragma once

#include "../shared/visualizer.h"
#include "../shared/draw_scope.h"
#include "../shared/client.h"
#include "../shared/new_vis_graph.h"
#include "../shared/common_algorithms/path.h"
#include "../shared/loader.h"


class visualizer_client : public client
{
public:
    visualizer_client (const graph_loader& loader, visualizer *pvis, draw_scope *pscope);
    ~visualizer_client();

    void on_mouse_move  (int x, int y);
    void on_mouse_down  (int x, int y, int button);
    void on_mouse_up    (int x, int y, int button);
    void on_key_down    (int key);
    void on_wheel       (int delta);
    void on_resize      (int width, int height);
    void on_paint       ();

    typedef boost::function<void(const vis_graph&, my_graph::vertex_id, my_graph::vertex_id, my_graph::path_map *, my_graph::path_map *, my_graph::path_map *)> algo_fn;

    void register_algorithm (const string &name, const algo_fn &fn);

private:

    void clear_lit ();
    void build_graph ();
    void update_lit ();
    void update_path_map(const my_graph::path_map &m, ib_id ib_dst, b_edge* pe_src);

    void draw_vertex (my_graph::vertex_id, int frame, const std::string& str);

    void test_hover (coord<int>);


    struct path_algorithm
    {
        string name;
        algo_fn fn;
    };

    void run_algorithm (const path_algorithm &algorithm);

    void mark_vertex (my_graph::vertex_id id);


    /*void update_border_set (my_graph::edge_id, bool rev);
    void run_dijkstra ();
    void run_bidijkstra ();
    void run_biastar ();
    void run_astar ();
    void run_astar_reaches ();
    void run_biastar_reaches ();

    void update_path (my_graph::vertex_id last);
    void update_bipath (my_graph::vertex_id last);

    template<class S>
    void run_search (S& s, const std::string& name);

    template<class S>
    void run_bisearch (S& s1, S& s2, const std::string& name);

    void crop ();*/
private:
    visualizer *pd_;
    draw_scope *pscope_;
    vb_id vb;
    ib_id ib, ib_lit1, ib_lit2, ib_path;
    vis_graph g;

    bool dragging_;
    coord<int> last_coord_;

    boost::optional<my_graph::vertex_id> hover_;
    boost::optional<my_graph::vertex_id> selected_;
    boost::optional<my_graph::vertex_id> start_, end_;


    list<path_algorithm> algorithms_;
    list<path_algorithm>::const_iterator algorithm_it_;

    my_graph::path_map lit1_, lit2_;
    my_graph::path_map path_;

    vis_coord mins, maxs;
    //my_graph::vertex_id path_last_;

    /*typedef unordered_set<my_graph::edge_id> edge_set;
    edge_set border_set_;
    DWORD event_time_;
    my_graph::edge_weight reach_limiter;

    bool draw_reaches_;*/

};
