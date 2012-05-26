#include "stdafx.h"


using my_graph::vertex_id;
using my_graph::edge_id;
using my_graph::vert_edge;

#include "shortcuts.h"

namespace 
{
    int g_mult_counter = 0;
    edge_weight g_max_diff = 0;
}
vertex_id g_marked_v1 = 0, g_marked_v2 = 0;

bool can_add_shortcut(reach_graph &g, vertex_id id1, vertex_id id2, const reach_edge_data &data)
{
    const reach_vertex &v = g.get_vertex(id1);

    for (auto it = v.out_begin(); it != v.out_end(); ++it)
    {
        if (it->v == id2)
        {
            const reach_edge &e = g.get_edge(it->e);
            if (e.get_data().len < data.len + 0.0000001)
                return false;

            edge_weight d = e.get_data().len - data.len;
            if (d > g_max_diff) 
            {
                g_marked_v1 = id1;
                g_marked_v2 = id2;
                g_max_diff = d;
            }

        }
    }
    if (id1 == 28415 && id2 == 28421)
        cout << "aaa success" << endl;
    return true;

}

boost::optional<edge_id> try_to_add_shortcut(reach_graph &g, vertex_id id1, vertex_id id2, const reach_edge_data &data)
{
    const reach_vertex &v = g.get_vertex(id1);

    for (auto it = v.out_begin(); it != v.out_end(); ++it)
    {
        if (it->v == id2)
        {
            const reach_edge &e = g.get_edge(it->e);
            if (e.get_data().len < data.len)
            {
                return boost::optional<edge_id> ();
            }

            edge_weight d = e.get_data().len - data.len;
            if (d > g_max_diff) 
            {
                g_marked_v1 = id1;
                g_marked_v2 = id2;
                g_max_diff = d;
            }
            ++g_mult_counter;
        }
    }
    return g.add_edge(id1, id2, data);
}

edge_id add_shortcuts(reach_graph &g, size_t degree, vector<shortcut> &dst, edge_id next_edge)
{
    const my_graph::edge_weight SHORTCUT_EPSILON = 0.000001;
    vector<bool> shortcutted(g.v_count(), false);
    vector<vert_edge> adj;

    size_t n_shortcuts = 0;
    for (auto it = g.v_begin(); it != g.v_end(); ++it)
    {
        bool aaa = false;
        const vertex_id id = g.get_vertex_id(it);
        
        adj.clear();
        for (auto adj_it = it->out_begin(); adj_it != it->out_end(); ++adj_it)
        {
            if (/*!shortcutted[adj_it->v]*/adj_it->v > id)
                adj.push_back(*adj_it);
        }

        if (id == 28414)
        {
            cout << "AAA size: " << adj.size() << endl;
            for (int i = 0; i < adj.size(); ++i)
                cout << " " << adj[i].v << endl;
            aaa = true;
        }

        if (adj.size() > degree || adj.size() < 2)
            continue;

        /*for (auto i = adj.begin(); i != adj.end(); ++i)
        {

            auto j = i;
            for (++j; j != adj.end(); ++j)
            {
                const reach_edge &e1 = g.get_edge(i->e);
                const reach_edge &e2 = g.get_edge(j->e);
                const reach_edge_data data(e1.data.len + e2.data.len);
                try_to_add_shortcut(g, i->v, j->v, data);
            }
        }*/
            
        for (size_t i = 0; i < adj.size(); ++i)
        {
            for (size_t j = i + 1; j < adj.size(); ++j)
            {
                // skip loops
                if (adj[i].v == id || adj[j].v == id)
                    continue;

                if (aaa)
                    cout << "aaa shortcut " << adj[i].v << ", " <<  adj[j].v << endl;
                
                const reach_edge &e1 = g.get_edge(adj[i].e);
                const reach_edge &e2 = g.get_edge(adj[j].e);
                reach_edge_data data(e1.data.len + e2.data.len - SHORTCUT_EPSILON);
                data.orig_id = next_edge;
                bool can = can_add_shortcut(g, adj[i].v, adj[j].v, data);

                if (can)
                {
                    const reach_vertex &v1 = g.get_vertex(adj[i].v);
                    const reach_vertex &v2 = g.get_vertex(adj[j].v);
                    
                    dst.push_back(shortcut(next_edge, 2));
                    shortcut &s = dst.back();

                   
                    
                    s.verts[0] = v1.get_data().orig_id;
                    s.verts[1] = it->get_data().orig_id;
                    s.verts[2] = v2.get_data().orig_id;

                    s.edges[0] = e1.get_data().orig_id;
                    s.edges[1] = e2.get_data().orig_id;

                    shortcutted[id] = true;
                    ++n_shortcuts;
                    ++next_edge;

                    g.add_edge(adj[i].v, adj[j].v, data);
                }
            }
        }

    }

    cout << "Shortcuts: " << n_shortcuts << endl;
    //cout << "Shortcuts mult counter: " << g_mult_counter << endl;
    cout << "Shortcuts max diff: " << g_max_diff << endl;

    return next_edge;
}

void add_shortcuts_temp (reach_graph &g)
{
    vector<bool> shortcutted(g.v_count(), false);

    my_graph::edge_weight shortcut_length;
    auto run = [&](vertex_id current, vertex_id prev) -> vertex_id
    {
        while (g.get_vertex(current).get_degree() == 2 && !shortcutted[current])
        {
            const reach_vertex &v = g.get_vertex(current);
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
        
        g.add_edge(start, end, reach_edge_data(shortcut_length));
        ++n_shortcuts;
    }
    cout << n_shortcuts << endl;
    cout << "Max length: " << max_length << endl;
}