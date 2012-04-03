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
        reach_preprocessor (graph &g, reach_map *pdst);
        void process_vertex(vertex_id id, edge_weight max_depth = std::numeric_limits<edge_weight>::max());
    private:
        void init_reach_map();
        void init_children_map();
        void build_children_map();
        void update_reach_map(vertex_id root);
        vertex_id update_reach_map_recursive(vertex_id current, vertex_id root);

        void remove_vertex(vertex_id id);
    private:
        graph *pg_;

        typedef vector<vertex_id> children_list;
        typedef unordered_map<vertex_id, children_list> children_map;
        children_map children_;

        path_map tree_;

        reach_map *preaches_;

        reach_map penalties;
    };


    reach_preprocessor::reach_preprocessor(graph &g, reach_map *pdst)
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
        penalties.clear();

        for (vis_graph::v_const_iterator it = pg_->v_begin(); it != pg_->v_end(); ++it)
        {
            (*preaches_)[it->first] = 0;
            penalties[it->first] = 0;
        }
    }

    void reach_preprocessor::process_vertex(vertex_id id, edge_weight max_depth)
    {
        tree_.clear();
        
        my_graph::dijkstra_class<vis_vertex_data, vis_edge_data> d (*pg_, get_vis_weight, tree_);
        d.init (id);
        while (!d.done())
        {
            vertex_id last = d.iterate();
            const path_vertex& last_pv = unordered_safe_find_const(tree_, last);
            if (last_pv.d > max_depth && last_pv.parent != id)
                break;
        }

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

    void reach_preprocessor::update_reach_map(vertex_id root)
    {
        update_reach_map_recursive(root, root);
    }

    vertex_id reach_preprocessor::update_reach_map_recursive(vertex_id current, vertex_id root)
    {
        const children_list &lst = unordered_safe_find_const(children_, current);
        const path_vertex &pv = unordered_safe_find_const(tree_, current);

        edge_weight max_height = 0;
        vertex_id max_descendant = current, descendant;
        for (children_list::const_iterator it = lst.begin(); it != lst.end(); ++it)
        {
            const path_vertex &child = unordered_safe_find_const(tree_, *it);
            descendant = update_reach_map_recursive(*it, root);
            edge_weight height = unordered_safe_find_const(tree_, descendant).d + pv.d;
            if (height > max_height)
            {
                max_height = height;
                max_descendant = descendant;
            }
        }
        

        edge_weight value = std::min(pv.d + unordered_safe_find_const(penalties, root), 
                                     max_height + unordered_safe_find_const(penalties, max_descendant));

        edge_weight &reach_ref = unordered_safe_find(*preaches_, current);
        if (reach_ref < value)
            reach_ref = value;

        return max_descendant;
    }

    void reach_preprocessor::remove_vertex(vertex_id id)
    {
        edge_weight reach = unordered_safe_find_const(*preaches_, id);
        const vis_vertex &v = pg_->get_vertex(id);
        for (vis_vertex::ve_const_iterator it = v.out_begin(); it != v.out_end(); ++it)
        {
            edge_weight &ref_penalty = unordered_safe_find(penalties, (*it)->get_v2().get_id());
            edge_weight new_penalty = reach + (*it)->get_data().len_;
            if (new_penalty > ref_penalty)
                ref_penalty = new_penalty;
        }

        for (vis_vertex::ve_const_iterator it = v.in_begin(); it != v.in_end(); ++it)
        {
            edge_weight &ref_penalty = unordered_safe_find(penalties, (*it)->get_v1().get_id());
            edge_weight new_penalty = reach + (*it)->get_data().len_;
            if (new_penalty > ref_penalty)
                ref_penalty = new_penalty;
        }

        pg_->delete_vertex(id);
    }

    void calculate_reaches(vis_graph& g, my_reaches::reach_map *pdst)
    {
        reach_preprocessor prep (g, pdst);
        int counter = 0;
        for (vis_graph::v_const_iterator it = g.v_begin(); it != g.v_end(); ++it, ++counter)
        {
            cout << counter << ". Calculating reaches for " << it->first << endl;
            prep.process_vertex(it->first);
        }
    }

    void calculate_reaches_partial(vis_graph& g, my_reaches::reach_map *pdst, edge_weight epsilon)
    {
        reach_preprocessor prep (g, pdst);
        while (g.v_count() > 0)
        {
            int counter = 0;
            for (vis_graph::v_const_iterator it = g.v_begin(); it != g.v_end(); ++it, ++counter)
            {
                if (counter % 1000 == 0)
                    cout << counter << ". Calculating reaches for " << it->first << endl;
                prep.process_vertex(it->first, 3*epsilon);
            }

            int removed_counter = 0;
            for (vis_graph::v_const_iterator it = g.v_begin(); it != g.v_end();)
            {
                if (unordered_safe_find_const(*pdst, it->first) < epsilon)
                {
                    ++removed_counter;
                    g.delete_vertex((it++)->first);
                }
                else
                    ++it;
            }
            cout << g.v_count() << " verts left" << endl;
            cout << removed_counter << " should have been removed" << endl;

            epsilon *= 3;
        }
    }
}