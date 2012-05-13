#include "stdafx.h"
#include "../shared/new_vis_graph.h"

vis_graph *add_shortcuts (const vis_graph &src)
{
    vis_graph *pdst = new vis_graph(src);

    for (auto it = pdst->v_begin(); it != pdst->v_end(); ++it)
    {
        //const my_graph::vertex_id id = it - pdst->v_begin();

        if (it->get_degree() > 3)
            continue;

        for (auto it1 = it->out_begin(); it1 != it->out_end(); ++it1)
        {
            for (auto it2 = it1; it2 != it->out_end(); ++it2)
            {
                const vis_edge &e1 = pdst->get_edge(it1->e);
                const vis_edge &e2 = pdst->get_edge(it2->e);
                const vis_edge_data data (e1.get_data().len + e2.get_data().len);
                pdst->add_edge(it1->v, it2->v, data);
            }
        }

    }

    return pdst;
}