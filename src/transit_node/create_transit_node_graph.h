#ifndef CREATE_TRANSIT_NODE_GRAPH_H
#define CREATE_TRANSIT_NODE_GRAPH_H
#include "tnr_utils.h"
namespace tnr {
	shared_ptr<vis_graph> create_tn_graph(const vis_graph &g, shared_ptr<access_map> access_nodes, shared_ptr<vertex_set> transit_nodes) {
		shared_ptr<vis_graph> new_graph(new vis_graph());
		unordered_map<vertex_id, vertex_id> mapping;
		vertex_set::const_iterator tn_iter = (*transit_nodes).begin();
		for(tn_iter; tn_iter != transit_nodes->end(); ++tn_iter) {
			vertex_id t_id= (*tn_iter);
			mapping[t_id] = new_graph->add_vertex(g.get_vertex(t_id).data);
		}

		tn_iter = transit_nodes->begin();

		for(tn_iter; tn_iter != transit_nodes->end(); ++tn_iter) {
			vertex_id t_id= (*tn_iter);
			
			unordered_map<vertex_id, access_info>::const_iterator an_iter = (*access_nodes)[t_id].begin();
			unordered_map<vertex_id, access_info>::const_iterator an_end_iter = (*access_nodes)[t_id].end();

			for(an_iter; an_iter != an_end_iter; ++an_iter) {
				vertex_id an_id = an_iter->first;
				edge_weight an_dist = an_iter->second.dist;
				if(t_id < an_id)
					new_graph->add_edge(mapping[t_id], mapping[an_id], vis_edge_data(an_dist));
			}
		}
		return new_graph;
	}
}


#endif