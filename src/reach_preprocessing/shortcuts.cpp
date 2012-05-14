#include "stdafx.h"
#include "../shared/new_vis_graph.h"

using my_graph::vertex_id;
using my_graph::edge_id;
using my_graph::vert_edge;

boost::optional<edge_id> try_to_add_shortcut(vis_graph &g, vertex_id id1, vertex_id id2, const vis_edge_data &data)
{
    const vis_vertex &v = g.get_vertex(id1);
    for (auto it = v.out_begin(); it != v.out_end(); ++it)
    {
        if (it->v == id2)
        {
            const vis_edge &e = g.get_edge(it->e);
            if (e.get_data().len < data.len)
                return boost::optional<edge_id> ();
        }
    }
    return g.add_edge(id1, id2, data);
}

void add_shortcuts(vis_graph &g, size_t degree)
{
    vector<bool> shortcutted(g.v_count(), false);
    vector<vert_edge> adj;

    size_t n_shortcuts = 0;
    for (auto it = g.v_begin(); it != g.v_end(); ++it)
    {
        const vertex_id id = g.get_vertex_id(it);
        
        adj.clear();
        for (auto adj_it = it->out_begin(); adj_it != it->out_end(); ++adj_it)
        {
            if (!shortcutted[adj_it->v])
                adj.push_back(*adj_it);
        }

        if (adj.size() > degree || adj.size() < 2)
            continue;

        /*for (auto i = adj.begin(); i != adj.end(); ++i)
        {

            auto j = i;
            for (++j; j != adj.end(); ++j)
            {
                const vis_edge &e1 = g.get_edge(i->e);
                const vis_edge &e2 = g.get_edge(j->e);
                const vis_edge_data data(e1.data.len + e2.data.len);
                try_to_add_shortcut(g, i->v, j->v, data);
            }
        }*/
            
        for (size_t i = 0; i < adj.size(); ++i)
        {
            for (size_t j = i + 1; j < adj.size(); ++j)
            {
                const vis_edge &e1 = g.get_edge(adj[i].e);
                const vis_edge &e2 = g.get_edge(adj[j].e);
                const vis_edge_data data(e1.data.len + e2.data.len);
                try_to_add_shortcut(g, adj[i].v, adj[j].v, data);
            }
        }

        shortcutted[id] = true;
        ++n_shortcuts;
    }

    cout << "Shortcuts: " << n_shortcuts << endl;
}

void add_shortcuts_temp (vis_graph &g)
{
    vector<bool> shortcutted(g.v_count(), false);

    my_graph::edge_weight shortcut_length;
    auto run = [&](vertex_id current, vertex_id prev) -> vertex_id
    {
        while (g.get_vertex(current).get_degree() == 2 && !shortcutted[current])
        {
            const vis_vertex &v = g.get_vertex(current);
            const vert_edge &ve = (v.get_adj()[0].v == prev) ? v.get_adj()[1] : v.get_adj()[0];

            shortcutted[current] = true;
            prev = current;
            current = ve.v;
            shortcut_length+=g.get_edge(ve.e).get_data().len;
        }
        shortcutted[current] = true;
        return current;
    };

    cout << "Adding shortcuts... ";
    size_t n_shortcuts = 0;
    my_graph::edge_weight max_length = 0;
    for (auto it = g.v_begin(); it != g.v_end(); ++it)
    {
        const vertex_id id = g.get_vertex_id(it);
        

        if (it->get_degree() != 2)
            continue;
        
        if (shortcutted[id])
            continue;
        

        shortcutted[id] = true;
        shortcut_length = 0;

        const vertex_id start = run(it->get_adj()[0].v, id);
        const vertex_id end   = run(it->get_adj()[1].v, id);

        if (shortcut_length > max_length)
            max_length = shortcut_length;
        
        g.add_edge(start, end, vis_edge_data(shortcut_length));
        ++n_shortcuts;
    }
    cout << n_shortcuts << endl;
    cout << "Max length: " << max_length << endl;
}