#ifndef TNR_LOADER_H
#define TNR_LOADER_H

#include "tnr_utils.h"
#include <fstream>
namespace tnr {
	class tnr_loader {
	public:
		 static shared_ptr<vertex_set> load_transit_nodes(string filename) {
			 shared_ptr<vertex_set> tn_ptr(new vertex_set());
			 std::ifstream in(filename.c_str());
			int id;
			while(in >> id) {
				tn_ptr->insert(id);
			}
			std::cout << "load " << tn_ptr->size() << " transit nodes" << std::endl;
			return tn_ptr;
		}
		static shared_ptr<access_map> load_access_nodes(string filename) {
			shared_ptr<access_map> ptr(new access_map());
			std::ifstream in(filename.c_str());
			string text;
			vertex_id v_id;
			size_t size;
			vertex_id an_id;
			edge_weight an_dist;
			vertex_id an_next;
			while(in >> text) {
				if(text == "vertex:") {
					in >> v_id;
					in >> text;
					if(text == "size:") {
						in >> size;
						for(int i = 0; i < size; ++i) {
							in >> an_id >> an_dist >> an_next;
							((*ptr)[v_id])[an_id] = access_info(an_next, an_dist);
						}
					} else { //if(text == "size:")
						std::cerr << "incorrect file format: (" << text << ")" << std::endl;
						return shared_ptr<access_map>();
					} //if(text == "size:") else	
				} else {
					std::cerr << "incorrect file format: (" << text << ")" << std::endl;
					return shared_ptr<access_map>();
				} //if(text == "vertex:")  else
			} //while(in >> text)
			std::cout << "access node loading is done!" << std::endl;
			return ptr;
		}
	};
}

#endif