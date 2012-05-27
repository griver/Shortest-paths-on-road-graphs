#ifndef DISTANCE_TABLE_H
#define DISTANCE_TABLE_H
#include "tnr_utils.h"
#include <fstream>


namespace tnr {
	typedef table_info table_cell;
	class dist_table {
		unordered_map<vertex_id, vertex_id> mapping;
		size_t size;
		table_cell**  table;
	public:
		dist_table(shared_ptr<vertex_set>& transit_nodes) {
			this->init_table(transit_nodes);
			this->init_maps(transit_nodes);
		}
		dist_table(unordered_map<vertex_id, vertex_id> mapping, table_cell** table) : mapping(mapping), table(table), size(mapping.size()){};
		
		edge_weight get_dist(vertex_id from, vertex_id to) const {
			this->check_id(from, to);
			vertex_id from_id = mapping.at(from);
			vertex_id to_id = mapping.at(to);
			return table[from_id][to_id].dist;
		}
		vertex_id get_next(vertex_id from, vertex_id to) const {
			this->check_id(from, to);
			vertex_id from_id = mapping.at(from);
			vertex_id to_id = mapping.at(to);
			return table[from_id][to_id].next;
		}
		void set_cell(vertex_id from, vertex_id to, vertex_id next, edge_weight dist) {
			this->check_id(from, to);
			vertex_id from_id = mapping.at(from);
			vertex_id to_id = mapping.at(to);
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
				//rmapping[i] = v_id;
			}
		}

		void check_id(vertex_id from, vertex_id to) const {
			if(mapping.count(from) == 0 || mapping.count(to) == 0) throw mapping_exception("mapping.count(from) == 0 || mapping.count(to) == 0");
			if(mapping.at(from) < 0 || mapping.at(from) >= size) throw mapping_exception("mapping.at(from) < 0 || mapping.at(from) >= size");
			if(mapping.at(to) < 0 || mapping.at(to) >= size) throw mapping_exception("mapping.at(to) < 0 || mapping.at(to) >= size");
		}
		friend void write_dist_table_to(dist_table const& table, string const & filname);
	};

	void write_dist_table_to(dist_table const& table, string const & filename) {
		unordered_map<vertex_id, vertex_id>::const_iterator map_iter = table.mapping.begin();
		unordered_map<vertex_id, vertex_id>::const_iterator map_end = table.mapping.end();
		std::ofstream outfile (filename.c_str());
		outfile << "size: " << table.size << endl;
		outfile << "mapping: " << endl;
		for(map_iter; map_iter != map_end; ++map_iter) {
			outfile << map_iter->first << " " << map_iter->second << endl;
		}
		outfile << "table: "  << endl;
		for(int i = 0; i < table.size; ++i) {
			for(int j = 0; j < table.size; ++j) {
				outfile << (table.table[i][j]).next << " " << (table.table[i][j]).dist; 
				if(j < (table.size - 1))
					outfile << " " ;
			}
			outfile << endl;
		}
		outfile.close();
	}
	
	shared_ptr<dist_table> read_dist_table_from(string const & filename) {
		std::ifstream in(filename.c_str());
		string str;
		
		size_t size;
		unordered_map<vertex_id, vertex_id> mapping;
		table_cell** table;
		

		in >> str;
		if(str != "size:") {
			cout << "reading error: (str != size:)" << endl;
			cout << "str = (" << str << ")"<<endl;
			return shared_ptr<dist_table>();
		}

		in >> size;

		table = new table_cell*[size];
		for(int i = 0; i < size; i++) {
			table[i] = new table_cell[size];
		}

		in >> str;

		if(str != "mapping:") {
			cout << "reading error: (str != mapping:)" << endl;
			cout << "str = (" << str << ")"<<endl;
			return shared_ptr<dist_table>();
		} else {
			cout << "mapping reading start" << endl;
		}
		
		vertex_id id, m_id; 
		for(int i = 0; i < size;  ++i) {
			in >> id >> m_id;
			mapping[id] = m_id;
		}
		if(size != mapping.size()) {
			cout << "reading error: size != mapping.size()" << endl;
			cout << "str = (" << size << ")"<<endl;
			return shared_ptr<dist_table>();
		} else {
			cout << "mapping reading complete" << endl;
		}


		in >> str;
		
		if(str != "table:") {
			cout << "reading error: (str != \"table:\")" << endl;
			cout << "str = (" << str << ")"<<endl;
			return shared_ptr<dist_table>();
		} else {
			cout << "table" << endl;
		}
		
	
		for(int i = 0; i < size; ++i) {
			for (int j = 0; j < size; ++j) {
				in >> table[i][j].next >> table[i][j].dist;  
			}
			if(i % 100 == 0) 
				cout << i << endl;
		}
		in.close();
		shared_ptr<dist_table> table_ptr(new dist_table(mapping, table));
		cout << "reading complete" << endl;
		return table_ptr;
	}
}

#endif