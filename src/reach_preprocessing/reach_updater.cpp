#include "stdafx.h"
#include "../shared/new_vis_graph.h"
#include "../shared/common_algorithms/path.h"

typedef my_graph::vertex_id vertex_id;
typedef my_graph::edge_id edge_id;
typedef my_graph::heap_vertex heap_vertex;
typedef my_graph::path_vertex path_vertex;
typedef my_graph::path_map path_map;
typedef my_graph::edge_weight edge_weight;
typedef my_graph::reach_map reach_map;

typedef unordered_map<vertex_id, vertex_id> candidate_map;
typedef unordered_map<vertex_id, edge_weight> threshold_map;

#include "reach_dijkstra.h"

DWORD g_tree_build_time, g_reach_update_time;
DWORD g_dijkstra_check_time;

class reach_updater
{
public:
    void calculate_reaches(const vis_graph &graph, vertex_id root, edge_weight epsilon, reach_map &ref_reaches);
    const path_map &get_tree() {return tree_;};
    const candidate_map &get_candidates() {return candidates_;};

private:
    void build_tree();
    void update_reaches_with_tree();
    edge_weight update_reaches_recursive(vertex_id id);


private:

    const vis_graph *pgraph_;
    unordered_set<vertex_id> saved_;
    path_map tree_;
    candidate_map candidates_;
    threshold_map thresholds_;
    reach_map *preaches_;
    edge_weight epsilon_;
    vertex_id root_;

    unordered_set<vertex_id> visited;
};


void reach_updater::calculate_reaches(const vis_graph &graph, vertex_id root, edge_weight epsilon, reach_map &ref_reaches)
{
    pgraph_ = &graph;
    root_ = root;
    epsilon_ = epsilon;
    preaches_ = &ref_reaches;

    saved_.clear();
    candidates_.clear();
    thresholds_.clear();

    DWORD start_time = timeGetTime();
    build_tree();
    g_tree_build_time += timeGetTime() - start_time;

    start_time = timeGetTime();
    update_reaches_with_tree();
    g_reach_update_time += timeGetTime() - start_time;
}

void reach_updater::build_tree()
{
    /*struct vert_checker
    {
        vert_checker (const candidate_map &candidates, const threshold_map &thresholds, const path_map &tree, edge_weight epsilon)
            : pcandidates_(&candidates), pthresholds_(&thresholds), ptree_(&tree), epsilon_(epsilon) {};

        bool operator() (const vertex_id id)
        {
            if (pcandidates_->count(id) == 0)
                return true;

            const edge_weight d = unordered_safe_find_const(*ptree_, id).d;
            const vertex_id &candidate = unordered_safe_find_const(*pcandidates_, id);
            const edge_weight dcand = unordered_safe_find_const(*ptree_, candidate).d;

        }

        candidate_map const * const pcandidates_;
        threshold_map const * const pthresholds_;
        path_map const * const ptree_;
        const edge_weight epsilon_;
    };
    vert_checker checker (candidates_, thresholds_, tree_, epsilon_);*/


    reach_dijkstra d (*pgraph_, root_, tree_, [&](const vertex_id &id) -> bool
    {
        if (candidates_.count(id) == 0)
            return true;
        
        const edge_weight &d = unordered_safe_find_const(tree_, id).d;
        const vertex_id &candidate = unordered_safe_find_const(candidates_, id);
        const edge_weight &dcand = unordered_safe_find_const(tree_, candidate).d;
        
        return (d - dcand <= epsilon_);
    });

    while (!d.done())
    {
        
        const vertex_id id = d.get_next();
        const path_map &border = d.get_border();
        const path_vertex &pv = unordered_safe_find_const(border, id);

        if (pv.parent.is_initialized())
        {
            const edge_weight thresh = (thresholds_.count(*pv.parent) == 0)
                                       ? pv.d
                                       : unordered_safe_find_const(thresholds_, *pv.parent);

            thresholds_[id] = thresh;
            
            if (candidates_.count(*pv.parent) == 0)
            {
                if (pv.d >= epsilon_)
                    candidates_[id] = id;
            }   
            else
            {
                const vertex_id &parent_cand = unordered_safe_find_const(candidates_, *pv.parent);
                if (parent_cand == *pv.parent
                    && pv.d >= epsilon_ && pv.d - thresh < epsilon_)
                {
                    candidates_[id] = id;
                }
                else
                    candidates_[id] = parent_cand;

            }
            
            /*if (pv.d >= epsilon_ && pv.d - thresh < epsilon_)
                candidates_[id] = id;
            else if (candidates_.count(*pv.parent))
                candidates_[id] = unordered_safe_find_const(candidates_, *pv.parent);*/
            /*if (candidates_.count(*pv.parent))
                candidates_[id] = unordered_safe_find_const(candidates_, *pv.parent);
            else if (pv.d > epsilon_)
                candidates_[id] = id;*/
        } 
        else 
        {
            assert(id == root_);
        }



        DWORD start_time = timeGetTime();
        const vertex_id id2 = d.iterate();
        g_dijkstra_check_time += timeGetTime() - start_time;
        assert(id2 == id);
    }

}

void reach_updater::update_reaches_with_tree()
{
    visited.clear();
    update_reaches_recursive(root_);
}

edge_weight reach_updater::update_reaches_recursive(vertex_id id)
{
    const vis_vertex &v = pgraph_->get_vertex(id);
    const path_vertex &pv = unordered_safe_find_const(tree_, id);

    if (visited.count(id) != 0)
    {
        cout << "LOOP DETECTED: " << id << ", parent " << *pv.parent << endl;
        throw 0;
    }
    visited.insert(id);
    
    edge_weight height = 0;
    for (vis_vertex::adj_iterator it = v.out_begin(); it != v.out_end(); ++it)
    {
        if (tree_.count (it->v) == 0)
            continue;
        
        const path_vertex &pv_child = unordered_safe_find_const(tree_, it->v);
        if (!pv_child.parent.is_initialized())
            continue;
        
        if (*(pv_child.parent) != id || *(pv_child.inc) != it->e)
            continue;

        /*if (it->v == 266927)
        {
            cout << "coming from " << id << endl;
            const vis_edge &e = pgraph_->get_edge(it->e);
            cout << "e " << it->e << ": len " << e.data.len << endl;
        }*/
        edge_weight new_height = update_reaches_recursive (it->v) + pv_child.d - pv.d;
        if (new_height > height)
            height = new_height;
    }

    edge_weight new_reach = std::min(pv.d, height);

    if (preaches_->count(id) == 0 || unordered_safe_find_const(*preaches_, id) < new_reach)
        (*preaches_)[id] = new_reach;

    return height;
}


/*void reach_preprocessor::run_dijkstra(vertex_id root)
{
    tree_.clear();
    candidates_.clear();
    saved_.clear();
    
    reach_dijkstra d(*pgraph_, root, tree_, boost::bind(&reach_preprocessor::check_vertex, this, _1));
    while (!d.done())
    {
        vertex_id id = d.iterate();
        
        boost::optional<vertex_id> candidate_id;

        if (candidates_.count(id) == 0)
        {
            if (unordered_safe_find_const(tree_, id).d > epsilon)
            {
                candidates_[id] = id;
                candidate_id.reset(id);
            }
        } 
        else
        {
            candidate_id.reset(unordered_safe_find_const(candidates_, id));
            if (unordered_safe_find_const(tree_, id).d - unordered_safe_find_const(tree_, *candidate_id).d > epsilon)
                saved_.insert(*candidate_id);
            
        }

        if (candidate_id.is_initialized())
        {
            const vis_vertex &v = pgraph_->get_vertex(id);
            for (vis_vertex::adj_iterator it = v.out_begin(); it != v.out_end(); ++it)
                candidates_[it->v] = *candidate_id;
        }
    }
}*/

/*void test_reach_updater(const vis_graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, path_map &ref_out2)
{
    const size_t N_THREADS = 2;
    const size_t verts_for_thread = (ref_graph.v_count() + N_THREADS - 1) / N_THREADS;

    const vis_vertex &v1 = ref_graph.get_vertex(start);
    const vis_vertex &v2 = ref_graph.get_vertex(end);

    vis_coord d = v2.data.c - v1.data.c;
    edge_weight dist = sqrt(d.x * d.x + d.y * d.y);

    reach_map reaches[N_THREADS];

#pragma omp parallel num_threads(N_THREADS)
    {
        reach_updater updater;

        size_t begin = verts_for_thread * omp_get_thread_num();
        size_t end = std::min(begin + verts_for_thread, ref_graph.v_count());

        for (vertex_id id = 0; id < ref_graph.v_count(); ++id)
            reaches[omp_get_thread_num()][id] = 0;

        for (vertex_id id = begin; id < end; ++id)
            updater.calculate_reaches(ref_graph, id, dist, reaches[omp_get_thread_num()]);
    }

    size_t removed_counter = 0;
    for (vertex_id id = 0; id < ref_graph.v_count(); ++id)
    {
        size_t i;
        for (i = 0; i < N_THREADS; ++i)
            if (unordered_safe_find_const(reaches[i], id) > dist)
                break;

        if (i == N_THREADS)
            ++removed_counter;
        else
        {
            const vis_vertex &v = ref_graph.get_vertex(id);
            for (vis_vertex::adj_iterator it = v.out_begin(); it != v.out_end(); ++it)
            {
                const path_vertex pv (it->v, 0, it->e, id);
                ref_out[it->v] = pv;
            }
        }
    }
    cout << "Removed " << removed_counter << " out of " << ref_graph.v_count() << " verts" << endl;
}*/

reach_updater g_updater;
path_map g_blue;
reach_map g_heights;
reach_map g_reaches;

void test_reach_updater(const vis_graph &ref_graph, vertex_id start, edge_weight dist, path_map &ref_out, path_map &ref_out2)
{
    const vis_vertex &v1 = ref_graph.get_vertex(start);

    cout << "Epsilon: " << dist << endl;

    reach_updater updater;
    reach_map reaches;
    
    updater.calculate_reaches(ref_graph, start, dist, reaches);

    //reach_map heights;
    reach_map &heights = g_heights;
    heights.clear();

    const path_map &tree = updater.get_tree();
    const candidate_map &candidates = updater.get_candidates();


    for (candidate_map::const_iterator it = candidates.begin(); it != candidates.end(); ++it)
    {
        edge_weight height = unordered_safe_find_const(tree, it->first).d
                           - unordered_safe_find_const(tree, it->second).d;

        if (heights.count(it->second) == 0 || height > unordered_safe_find_const(heights, it->second))
            heights[it->second] = height;
    }
    
    for (candidate_map::const_iterator it = candidates.begin(); it != candidates.end(); ++it)
    {
        edge_weight height = unordered_safe_find_const(tree, it->first).d
            - unordered_safe_find_const(tree, it->second).d;

        ref_out[it->first] = unordered_safe_find_const(tree, it->first);
        if (height == unordered_safe_find_const(heights, it->second))
            ref_out2[it->first] = unordered_safe_find_const(tree, it->first);

    }
   
    /*for (candidate_map::const_iterator it = candidates.begin(); it != candidates.end(); ++it)
    {
        if (it->first != it->second)
            continue;

        const edge_weight reach = unordered_safe_find_const(reaches, it->first);
        if (reach >= dist)
            continue;

        const edge_weight height = unordered_safe_find_const(heights, it->first);
        const edge_weight depth = unordered_safe_find_const(tree, it->first).d;
        cout << "! " << it->first << ": " << reach << " (" << depth << " : " << height << ")" << endl;
        
    }*/

    g_blue = ref_out2;
    g_updater = updater;
    g_reaches = reaches;

}

void test_reach_tester (const vis_graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, path_map &ref_out2)
{
    const candidate_map &candidates = g_updater.get_candidates();
    const path_map &tree = g_updater.get_tree();
    
    ref_out2 = g_blue;
    if (candidates.count(start) == 0)
        return;

    vertex_id cand_id = unordered_safe_find_const(candidates, start);
    ref_out[cand_id] = unordered_safe_find_const(tree, cand_id);

    cout << "Height: " << unordered_safe_find_const(g_heights, cand_id) << endl;
    cout << "Depth: " << unordered_safe_find_const(tree, cand_id).d << endl;
    cout << "Reach: " << unordered_safe_find_const(g_reaches, cand_id) << endl;
    
    
    if (cand_id == start)
        cout << "AAAA SELF-CANDIDATE!!!" << endl;
}

void add_star (const vis_graph &ref_graph, vertex_id id, path_map &ref_out)
{
    const vis_vertex &v = ref_graph.get_vertex(id);
    for (vis_vertex::adj_iterator it = v.out_begin(); it != v.out_end(); ++it)
    {
        const path_vertex pv (it->v, 0, it->e, id);
        ref_out[it->v] = pv;
    }
}

void test_reach_updater_new(const vis_graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, path_map &ref_out2)
{
    const vis_vertex &v1 = ref_graph.get_vertex(start);
    const vis_vertex &v2 = ref_graph.get_vertex(end);

    vis_coord d = v2.data.c - v1.data.c;
    edge_weight dist = sqrt(d.x * d.x + d.y * d.y);
    cout << "Epsilon: " << dist << endl;

    
    size_t counter = 0;
    reach_map big_reaches;
    for (vis_graph::v_const_iterator it = ref_graph.v_begin(); it != ref_graph.v_end(); ++it)
    {
        vertex_id id = it - ref_graph.v_begin();

        reach_updater big_updater;
        big_updater.calculate_reaches(ref_graph, start, 1000.0f, big_reaches);
        
        if (counter % 100 == 0)
            cout << counter << " verts processed" << endl;
        ++counter;

    }

    cout << "Big reaches calculated" << endl;

    reach_map little_reaches;
    for (vis_graph::v_const_iterator it = ref_graph.v_begin(); it != ref_graph.v_end(); ++it)
    {
        vertex_id id = it - ref_graph.v_begin();

        reach_updater little_updater;
        little_updater.calculate_reaches(ref_graph, id, dist, little_reaches);
    }



    for (reach_map::const_iterator it = little_reaches.begin(); it != little_reaches.end(); ++it)
    {
        edge_weight little_reach = it->second;
        edge_weight big_reach = unordered_safe_find_const(big_reaches, it->first);
        if (big_reach >= dist && little_reach < dist)
            cout << "ERROR: " << it->first << ": " << big_reach << " vs " << little_reach << endl;

        if (little_reach >= dist)
            add_star(ref_graph, it->first, ref_out);
    }



}

void test_candidates(const vis_graph &ref_graph, vertex_id start, vertex_id end, path_map &ref_out, path_map &ref_out2)
{
    const vis_vertex &v1 = ref_graph.get_vertex(start);
    const vis_vertex &v2 = ref_graph.get_vertex(end);

    vis_coord d = v2.data.c - v1.data.c;
    edge_weight dist = sqrt(d.x * d.x + d.y * d.y);
    cout << "Epsilon: " << dist << endl;

    reach_map little_reaches;
    reach_updater little_updater;
    little_updater.calculate_reaches(ref_graph, start, dist, little_reaches);

    reach_map big_reaches;
    reach_updater big_updater;
    big_updater.calculate_reaches(ref_graph, start, 1000.0f, big_reaches);

    const path_map &tree = little_updater.get_tree();
    const candidate_map &cand = little_updater.get_candidates();
    for (path_map::const_iterator it = tree.begin(); it != tree.end(); ++it)
    {
        edge_weight little_reach = unordered_safe_find_const(little_reaches, it->first);
        edge_weight big_reach = unordered_safe_find_const(big_reaches, it->first);
        if (cand.count(it->first) == 0 || unordered_safe_find_const(cand, it->first) == it->first)
        {
            assert (little_reach >= dist || big_reach < dist);
            
            if (big_reach >= dist && little_reach < dist)
                cout << "ERROR: " << it->first << ": " << big_reach << " vs " << little_reach << endl;
        
        }

        if (little_reach < dist)
            ref_out[it->first] = it->second;
        else
            ref_out2[it->first] = it->second;

    }

}

vis_graph *cut_graph(const vis_graph &src, const reach_map &reaches, edge_weight epsilon)
{
    vis_graph *pdst = new vis_graph();
    vis_graph &dst = *pdst;
    
    unordered_map<vertex_id, vertex_id> verts_map;
    verts_map.rehash(src.v_count());

    for (vis_graph::v_const_iterator it = src.v_begin(); it != src.v_end(); ++it)
    {
#if defined (GRAPH_ITER_WRAPPER_CHECK)
#error Fixme: V_ITER_WRAPPER normal iterator wrapper needed
#endif 
        const vertex_id src_id = it - src.v_begin();

        const edge_weight &reach = unordered_safe_find_const(reaches, src_id);
        if (reach >= epsilon)
        {
            const vertex_id dst_id = dst.add_vertex(it->get_data());
            verts_map[src_id] = dst_id;
        }
    }

    for (vis_graph::v_const_iterator it = src.v_begin(); it != src.v_end(); ++it)
    {
#if defined (GRAPH_ITER_WRAPPER_CHECK)
#error Fixme: V_ITER_WRAPPER normal iterator wrapper needed
#endif 
        const vertex_id src_id = it - src.v_begin();

        auto m1 = verts_map.find(src_id);
        if (m1 == verts_map.end())
            continue;

        const vis_vertex &src_v = *it;
        for (vis_vertex::adj_iterator adj = src_v.out_begin(); adj != src_v.out_end(); ++adj)
        {
            auto m2 = verts_map.find(adj->v);
            if (m2 == verts_map.end())
                continue;

            
            const vis_edge &e = src.get_edge(adj->e);
            dst.add_edge(m1->second, m2->second, e.get_data());
        }

        // erase traversed verts so that edges won't be added twice
        verts_map.erase(m1);
    }

    
    return pdst;
}

vis_graph *run_reaches_update(const vis_graph &ref_graph, edge_weight dist)
{
    const size_t PRINT_EACH_VERTS = 10000;
    const DWORD PRINT_EACH_MS = 30000;

    cout << "Epsilon: " << dist << endl;

    g_dijkstra_check_time = g_tree_build_time = g_reach_update_time = 0;
    
    
    DWORD last_time = timeGetTime();
    reach_map little_reaches;
    size_t counter = 0;
    for (vis_graph::v_const_iterator it = ref_graph.v_begin(); it != ref_graph.v_end(); ++it)
    {
        vertex_id id = ref_graph.get_vertex_id(it);

        reach_updater little_updater;
        little_updater.calculate_reaches(ref_graph, id, dist, little_reaches);

        ++counter;
        DWORD new_time = timeGetTime();
        if (new_time - last_time > PRINT_EACH_MS || counter >= PRINT_EACH_VERTS)
        {
            cout << counter << " verts processed" << endl;
            counter = 0;
            last_time = new_time;
        }
    }

    cout << "Times: " << g_tree_build_time << ", " << g_reach_update_time << endl;
    cout << "Dijkstra check time: " << g_dijkstra_check_time << endl;

    counter = 0;
    for (reach_map::const_iterator it = little_reaches.begin(); it != little_reaches.end(); ++it)
    {
        if (it->second >= dist)
            ++counter;
    }

    cout << "Left " << counter << " out of " << ref_graph.v_count() << " verts" << endl;
    
    return NULL;
    //return cut_graph(ref_graph, little_reaches, dist);
}


void draw_circle (const vis_graph &ref_graph, vertex_id start, edge_weight dist, path_map &ref_out, path_map &ref_out2)
{
    reach_updater updater;
    reach_map reaches;
    DWORD start_time = timeGetTime();
    updater.calculate_reaches(ref_graph, start, dist, reaches);
    DWORD elapsed_time = timeGetTime() - start_time;

    cout << "Elapsed time: " << elapsed_time << " ms." << endl;

    const path_map &tree = updater.get_tree();
    const candidate_map &candidates = updater.get_candidates();

    for (auto it = candidates.begin(); it != candidates.end(); ++it)
    {
        ref_out2[it->second] = unordered_safe_find_const(tree, it->second);
    }

    ref_out = tree;

}

void test_circles (const vis_graph &g)
{
    const int N = 100;

    DWORD times = 0;
    reach_map reaches;
    
    cout << "Testing: " << endl;
    for (int i = 0; i < N; ++i)
    {
        const size_t rnd = rand() * RAND_MAX + rand();
        vertex_id id = rnd % g.v_count();

        cout << id << " ";

        reach_updater updater;
        const DWORD start_time = timeGetTime();
        updater.calculate_reaches(g, id, 0.01, reaches);
        times += timeGetTime() - start_time;
    }

    const DWORD avg_time = times / N;
    cout << endl << "Avg time: " << avg_time << endl;
}