#ifndef DISTANCE_TABLE_H
#define DISTANCE_TABLE_H
#include "tnr_utils.h"


namespace tnr {
	typedef access_info table_cell;
	struct dist_table {
		unordered_map<vertex_id, vertex_id> mapping;
		unordered_map<vertex_id, vertex_id> rmapping;
		int size;
		table_cell**  table;
	public:
		dist_table(shared_ptr<vertex_set>& transit_nodes) {
			this->init_table(transit_nodes);
			this->init_maps(transit_nodes);
		}
		
		edge_weight get_dist(vertex_id from, vertex_id to) const {
			this->check_id(from, to);
			vertex_id from_id = mapping[from];
			vertex_id to_id = mapping[to];
			return table[from_id][to_id].dist;
		}
		vertex_id get_next(vertex_id from, vertex_id to) const {
			this->check_id(from, to);
			vertex_id from_id = mapping[from];
			vertex_id to_id = mapping[to];
			return table[from_id][to_id].next;
		}
		void set_cell(vertex_id from, vertex_id, vertex_id next, edge_weight dist) {
			this->check_id(from, to);
			vertex_id from_id = mapping[from];
			vertex_id to_id = mapping[to];
			table[from_id][to_id] = table_cell(next, dist);
		}

		virtual ~dist_table() {
			for(int i =0; i < size; ++i)
				delete[] table[i];
			delete[] table;
		}
	private:
		void init_table(shared_ptr<vertex_set>& transit_nodes) {
			size = transit_nodes->size();
			table = new table_cell*[size];

			for(int i =0; i< size; i++) {
				table[i] = new table_cell[size];
			}
		}

		void init_maps(shared_ptr<vertex_set>& transit_nodes) {
			vertex_set::const_iterator iter = (*transit_nodes).begin();
			vertex_set::const_iterator end = (*transit_nodes).end();
			int i = 0;
			for(iter; iter != end; ++iter, ++i) {
				vertex_id v_id = (*iter);
				mapping[v_id] = i;
				rmapping[i] = v_id;
			}
		}

		void check_id(vertex_id from, vertex_id to) const {
			if(mapping.count(from) == 0 || mapping.count(to) == 0) throw mapping_exception("mapping.count(from) == 0 || mapping.count(to) == 0");
			if(mapping.at(from) < 0 || mapping.at(from) >= size) throw mapping_exception("mapping.at(from) < 0 || mapping.at(from) >= size");
			if(mapping.at(to) < 0 || mapping.at(to) >= size) throw mapping_exception("mapping.at(to) < 0 || mapping.at(to) >= size");
		}
	};
	
}

#endif