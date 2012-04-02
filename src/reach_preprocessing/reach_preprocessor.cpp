#include "stdafx.h"
#include "reach_preprocessor.h"

namespace my_reaches 
{

    class reach_preprocessor
    {
    private:
        typedef vis_graph graph;
        typedef vis_vertex vertex;
        typedef vis_edge edge;
    public:
        reach_preprocessor (const graph &g, reach_map *pdst);
        void process_vertex(vertex_id id);
    private:
        void init_reach_map();
        void init_children_map();
        void build_children_map();
        edge_weight update_reach_map(vertex_id root);
    private:
        const graph *pg_;

        typedef vector<vertex_id> children_list;
        typedef unordered_map<vertex_id, children_list> children_map;
        children_map children_;

        path_map tree_;

        reach_map *preaches_;
    };


    reach_preprocessor::reach_preprocessor(const graph &g, reach_map *pdst)
        : pg_(&g)
        , preaches_(pdst)
    {
        init_reach_map();
        init_children_map();
        tree_.rehash(g.v_count());
    }

    void reach_preprocessor::init_children_map()
    {
        children_list lst;
        for (vis_graph::v_const_iterator it = pg_->v_begin(); it != pg_->v_end(); ++it)
            children_[it->first] = lst;
    }

    void reach_preprocessor::init_reach_map()
    {
        preaches_->clear();
        for (vis_graph::v_const_iterator it = pg_->v_begin(); it != pg_->v_end(); ++it)
            (*preaches_)[it->first] = 0;
    }

    void reach_preprocessor::process_vertex(vertex_id id)
    {
        tree_.clear();
        run_full_dijkstra(get_vis_weight, *pg_, id, &tree_);
        build_children_map();
        update_reach_map(id);
    }

    void reach_preprocessor::build_children_map()
    {
        // Clear the children map values
        for (children_map::iterator it = children_.begin(); it != children_.end(); ++it)
        {
            it->second.clear();
        }

        // Store the children
        for (path_map::const_iterator it = tree_.begin(); it != tree_.end(); ++it)
        {
            const path_vertex &pv = it->second;
            if (!pv.parent.is_initialized())
                continue;

            assert (children_.find(*pv.parent) != children_.end());
            children_list &lst = children_.find(*pv.parent)->second;

            lst.push_back(it->first);
        }
    }

    edge_weight reach_preprocessor::update_reach_map(vertex_id root)
    {
        const children_list &lst = unordered_safe_find_const(children_, root);
        const path_vertex &pv = unordered_safe_find_const(tree_, root);

        edge_weight max_height = 0;
        for (children_list::const_iterator it = lst.begin(); it != lst.end(); ++it)
        {
            const path_vertex &child = unordered_safe_find_const(tree_, *it);
            edge_weight height = update_reach_map(*it) + (child.d - pv.d);
            if (height > max_height)
                max_height = height;
        }


        edge_weight value = std::min(pv.d, max_height);

        edge_weight &reach_ref = unordered_safe_find(*preaches_, root);
        if (reach_ref < value)
            reach_ref = value;

        return max_height;
    }

    void calculate_reaches(const vis_graph& g, my_reaches::reach_map *pdst)
    {
        reach_preprocessor prep (g, pdst);
        int counter = 0;
        for (vis_graph::v_const_iterator it = g.v_begin(); it != g.v_end(); ++it, ++counter)
        {
            cout << counter << ". Calculating reaches for " << it->first << endl;
            prep.process_vertex(it->first);
        }
    }
}