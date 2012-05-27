
#include "stdafx.h"


#include "../shared/osm_loader.h"

#include "../tnr_utils/tnr_loader.h"
#include "../tnr_utils/grid_info.h"
#include "tnr_path_finder.h"

shared_ptr<tnr::vertex_set> tn;
shared_ptr<tnr::access_map> am;
shared_ptr<tnr::dist_table> dt;
shared_ptr<tnr::grid_info> grid;

shared_ptr<tnr::tnr_path_finder> finder;


bool is_far_enouth(tnr::coord_t const & a, tnr::coord_t const & b) {
	if(abs(grid->get_x_cell(a.x) - grid->get_x_cell(b.x)) > 4) 
		return true;
	if(abs(grid->get_y_cell(a.y) - grid->get_y_cell(b.y)) > 4)
		return true;
	return false;
}


void tnr_find(tnr::vertex_id start, tnr::vertex_id end, tnr::path_map *ppath, vis_graph const &g) {	
	
	if(!is_far_enouth(g.get_vertex(start).data.c, g.get_vertex(end).data.c)) {
		cout << " sorry! to close!!" << endl;
	} else {
	    finder->search(start, end);
	    cout << "dist to target: " << (*ppath)[end].d << endl;
    }
}



int main() {
    
    scoped_ptr<vis_graph> pgraph(new vis_graph());
    vis_coord mins, maxs;
    char c;
    load_osm("../../osm_maps/ireland.osm", *pgraph, mins, maxs);
    tnr::path_map path;
    
    //tn = tnr::tnr_loader::load_transit_nodes("../../transit_nodes_data/ireland.stn");
    cout << "----------load_access_nodes-------------" << endl;    
    am = tnr::tnr_loader::load_access_nodes("../../transit_nodes_data/ireland.sian");
    cout << "----------load_distance_table-------------" << endl;
    dt = tnr::read_dist_table_from("../../transit_nodes_data/ireland.sidt");
    cout << "----------calculate_grid_info-------------" << endl;
    grid = shared_ptr<tnr::grid_info>(new tnr::grid_info(*pgraph, 100, 100));
    cout << "-------------------done!------------------" << endl;
    
    finder.reset(new tnr::tnr_path_finder(*pgraph, am, dt, &path));

    tnr::vertex_id x, y;
    for(int i = 0; i < 50; ++i) {
        cout << "введите id начала и конца: " << endl;
        std::cin >> x >> y;
        tnr_find(x, y, &path, *pgraph);        
    }

    return 0;
}
