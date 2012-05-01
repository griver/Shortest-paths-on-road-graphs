// reach_preprocessing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../visualizer_client/vis_graph.h"
#include "old_loader.h"
#include "reach_preprocessor.h"

using my_reaches::reach_map;
using my_reaches::calculate_reaches_partial;
using my_reaches::calculate_reaches;

int main(int argc, char* argv[])
{
	string filename = "nw_cropped";
    
    vis_graph g;
    my_graph::load_graph (filename + ".co", g);
    my_graph::load_graph (filename + ".gr", g);
    my_graph::load_graph (filename + ".re", g);

    
    vector<int> reach_stats;
    for (vis_graph::v_const_iterator it = g.v_begin(); it != g.v_end(); ++it)
    {
        edge_weight reach = (it->second).get_data().reach;
        size_t index = reach / 1000;
        if (index >= reach_stats.size())
            reach_stats.resize(index + 1, 0);
        ++reach_stats[index];
    }
    
    std::ofstream out ("reach_stats.txt");
    for (size_t i = 0; i < reach_stats.size(); ++i)
        out << i << ": " << reach_stats[i] << endl;

    //my_reaches::reach_map reaches;
    //calculate_reaches_partial(g, &reaches);

    /*cout << endl << "Reaches:" << endl;
    for (reach_map::const_iterator it = reaches.begin(); it != reaches.end(); ++it)
        cout << it->first << ": " << it->second << endl;*/

    /*vector<int> lens (10, 0);
    edge_weight max_len = 0, len;
    for (vis_graph::e_const_iterator it = g.e_begin(); it != g.e_end(); ++it)
    {
        len = it->second.get_data().len_;
        if (len > max_len)
            max_len = len;
        
        size_t id = len / 1000;
        if (id >= lens.size())
            lens.resize(id + 1, 0);
        ++lens[len / 1000];
    }
    for (size_t i = 0; i < lens.size(); ++i)
        cout << i << ": " << lens[i] << endl;
    cout << "Max_len: " << max_len << endl;*/   
    return 0;
}

