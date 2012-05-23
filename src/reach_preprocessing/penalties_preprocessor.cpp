#include "stdafx.h"

/*

class penalties_preprocessor
{
public:
    typedef reach_vertex vertex;
    typedef reach_edge edge;
    typedef reach_graph graph;
public:
    bool done() const;
    void interate();

private:
    shortcut_id add_shortcut(vertex_id v1, edge_id e12, vertex_id v2, edge_id e23, vertex_id v3);
    shortcut_id add_shortcut(const path_map &tree, vertex_id id);
private:
    typedef unordered_map<osm_vertex_id, edge_weight> osm_reach_map;


private:
    boost::scoped_ptr<reach_graph> pgraph_;
    osm_reach_map reaches_;
    vector<reach_shortcut> shortcuts_;

};

shortcut_id penalties_preprocessor::add_shortcut(vertex_id v1id, edge_id e12id, vertex_id v2id, edge_id e23id, vertex_id v3id)
{
    const vertex &v1 = pgraph_->get_vertex(v1id);
    const vertex &v2 = pgraph_->get_vertex(v2id);
    const vertex &v3 = pgraph_->get_vertex(v3id);

    const edge &e12 = pgraph_->get_edge(e12id);
    const edge &e23 = pgraph_->get_edge(e23id);

    reach_shortcut shortcut;
    
    if (e12.get_data().is_shortcut())
    {
        const shortcut_id e12_shortcut_id = e12.get_data().shortcut;

        assert(shortcuts_[e12_shortcut_id].front() == v1.get_data().osm_id);
        assert(shortcuts_[e12_shortcut_id].back()  == v2.get_data().osm_id);
        shortcut.insert(shortcut.end(), shortcuts_[e12_shortcut_id].begin(), shortcuts_[e12_shortcut_id].end());
    }
    else
    {
        shortcut.push_back(v1.get_data().osm_id);
        shortcut.push_back(v2.get_data().osm_id);
    }

    if (e23.get_data().is_shortcut())
    {
    }
    else
    {

    }

}



*/