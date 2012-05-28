#pragma once


using my_graph::vertex_id;
using my_graph::edge_id;
using my_graph::path_map;
using my_graph::path_vertex;

//---tnr--
using tnr::dist_table;
using tnr::access_map;
using tnr::grid_info;
//--/tnr--

class routes
{
public:
    routes (string const  &osm_filename, string const &an_filename, string const &dt_filename);
    bool get_path(vertex_id s, vertex_id t, vector<vis_coord> &dst);
    vertex_id get_vertex_by_coord(const vis_coord& c) const;
    
    const vis_graph &get_graph() {return graph;}
    const vis_coord &get_mins() {return mins;}
    const vis_coord &get_maxs() {return maxs;}
    //---tnr--
    dist_table const & get_distance_table() {return *dt;}
    access_map const & get_access_map() {return *am;}
    //--/tnr--
    
private:
    inline bool is_far_enouth(vis_coord const & a, vis_coord const & b);
private:
    vis_coord mins, maxs;
    vis_graph graph;
    path_map tree;

    //---tnr--
    shared_ptr<dist_table> dt;
    shared_ptr<access_map> am;
    shared_ptr<grid_info> grid;
    shared_ptr<tnr::tnr_path_finder> finder;
    //--/tnr--        
};
