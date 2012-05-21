#include "../shared/graph_base.h"
#include "../shared/new_vis_graph.h"

template<typename V, typename E>
class osm_loader
{
public:
    typedef V vertex_data;
    typedef E edge_data;
    typedef typename my_graph::graph_base<V, E> graph;
    typedef typename my_graph::vertex_base<V, E> vertex;
    typedef typename my_graph::edge_base<V, E> edge;
private:
    typedef rapidxml::xml_document<> xml_document;
    typedef rapidxml::xml_node<> xml_node;
    typedef size_t xml_id;
    typedef my_graph::vertex_id vertex_id;

    static const size_t PRINT_EVERY = 100000;
public:
    osm_loader(const string& path, graph &ref_graph);
    void load();

    void print_stats();

    const vis_coord &get_mins() const {return mins;}
    const vis_coord &get_maxs() const {return maxs;}

private:
    void load_way(xml_node *root);
    void load_edge(xml_node *node1, xml_node *node2);
    void load_verts();
    void load_edges();
    void update_borders(const vis_coord &coord);
private:
    boost::scoped_array<char> text_;
    xml_document doc_;
    xml_node *root_;
    graph *pgraph_;
    unordered_map<xml_id, vertex_id> verts_map_;

    vis_coord mins, maxs;
};

template<typename V, typename E>
osm_loader<V, E>::osm_loader(const string& path, graph &ref_graph)
: pgraph_(&ref_graph)
, mins(std::numeric_limits<vis_coord::value_type>::max(), 
       std::numeric_limits<vis_coord::value_type>::max())
, maxs(-std::numeric_limits<vis_coord::value_type>::max(), 
       -std::numeric_limits<vis_coord::value_type>::max())
{
    ifstream src;
    src.exceptions(ifstream::failbit | ifstream::badbit);
    src.open(path.c_str(), std::ios_base::in | std::ios_base::binary);

    src.seekg(0, std::ios::end);
    size_t size = static_cast<size_t>(src.tellg());
    src.seekg(0);

    cout << "Loading " << path << endl;
    text_.reset(new char[size+1]);
    src.read(text_.get(), size);
    text_[size] = '\0';

    cout << "Parsing " << path << endl;
    doc_.parse<0>(text_.get());
    root_ = doc_.first_node("osm");

    pgraph_->clear();
    //pgraph_->reserve(1000000, 2000000);
}

template<typename V, typename E>
void osm_loader<V, E>::load()
{
    load_verts();
    load_edges();
}

template<typename V, typename E>
void osm_loader<V, E>::load_verts()
{
    cout << "Loading verts" << endl;

    const char node_name[] = "node";
    for (xml_node *node = root_->first_node(node_name); node != NULL;
                   node = node->next_sibling(node_name))
    {
        const xml_id id = atol(node->first_attribute("id")->value());

        const double x = atof(node->first_attribute("lat")->value());
        const double y = atof(node->first_attribute("lon")->value());

        const vis_coord coord (static_cast<vis_coord::value_type>(x), static_cast<vis_coord::value_type>(y));

        vertex_data data (coord);
        data.orig_id = id;

        verts_map_[id] = pgraph_->add_vertex(data);
        update_borders(coord);

        if (pgraph_->v_count() % PRINT_EVERY == 0)
            cout << pgraph_->v_count() << " verts loaded" << endl;
    }
}

template<typename V, typename E>
void osm_loader<V, E>::load_edges()
{
    cout << "Loading edges" << endl;

    const char node_name[] = "way";
    for (xml_node *node = root_->first_node(node_name); node != NULL;
                   node = node->next_sibling(node_name))
    {
        load_way (node);
    }

}

template<typename V, typename E>
void osm_loader<V, E>::load_way(xml_node *root)
{
    const char node_name[] = "nd";
    xml_node *current = root->first_node(node_name);
    xml_node *next = current->next_sibling(node_name);
    while (next != NULL)
    {
        load_edge(current, next);
        current = next;
        next = next->next_sibling(node_name);
    }
}

template<typename V, typename E>
void osm_loader<V, E>::load_edge(xml_node *node1, xml_node *node2)
{
    const xml_id xml_vid1 = atol(node1->first_attribute("ref")->value());
    const xml_id xml_vid2 = atol(node2->first_attribute("ref")->value());

    if (verts_map_.count(xml_vid1) == 0 || verts_map_.count(xml_vid2) == 0)
        return;

    const vertex_id vid1 = unordered_safe_find_const(verts_map_, xml_vid1);
    const vertex_id vid2 = unordered_safe_find_const(verts_map_, xml_vid2);

    const vertex &v1 = pgraph_->get_vertex(vid1);
    const vertex &v2 = pgraph_->get_vertex(vid2);

    const vis_coord c = v1.get_data().c - v2.get_data().c;
    const my_graph::edge_weight len = static_cast<my_graph::edge_weight>(::sqrt (c.x*c.x + c.y*c.y));

    const edge_data data (len);

    pgraph_->add_edge(vid1, vid2, data);

    if (vid1 == vid2)
        cout << "Loop detected: "  << vid1 << endl;

    if (pgraph_->e_count() % PRINT_EVERY == 0)
        cout << pgraph_->e_count() << " edges loaded" << endl;
}

template<typename V, typename E>
void osm_loader<V, E>::update_borders(const vis_coord &coord)
{
    if (coord.x < mins.x) mins.x = coord.x;
    if (coord.y < mins.y) mins.y = coord.y;
    if (coord.x > maxs.x) maxs.x = coord.x;
    if (coord.y > maxs.y) maxs.y = coord.y;
}

template<typename V, typename E>
void load_osm(const string &path, my_graph::graph_base<V, E> &ref_graph, vis_coord &ref_mins, vis_coord &ref_maxs)
{
    osm_loader<V, E> loader (path, ref_graph);
    loader.load();
    ref_mins = loader.get_mins();
    ref_maxs = loader.get_maxs();
}