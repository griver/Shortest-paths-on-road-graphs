#include "stdafx.h"
#include "../shared/new_vis_graph.h"

using my_graph::vertex_id;

//const double SCALE = 1.0;
const size_t MAX_VERTS = std::numeric_limits<size_t>::max();

class osm_loader
{
private:
    typedef rapidxml::xml_document<> xml_document;
    typedef rapidxml::xml_node<> xml_node;
    typedef size_t xml_id;
public:
    osm_loader(const string& path, vis_graph *pgraph);
    void load_verts();
    void load_edges();
    void print_stats();

    const vis_coord &get_mins() const {return mins;}
    const vis_coord &get_maxs() const {return maxs;}

private:
    void prepare_verts(unordered_set<xml_id> &verts_pending);

    void load_bounds();
    void load_way(xml_node *root, unordered_set<xml_id> &verts_pending);
    void load_edge(xml_node *node1, xml_node *node2, bool oneway, unordered_set<xml_id> &verts_pending);
    bool check_if_highway(xml_node *root);
    bool check_if_oneway(xml_node *root);

    vis_vertex& lazy_get_vertex(xml_id id);
    void update_edge_weights();
private:
    boost::scoped_array<char> text_;
    xml_document doc_;
    vis_graph *pgraph_;
    
    vis_coord mins, maxs;
    size_t vert_counter_;
    vector<int> way_stats_, degrees_stats_;
    unordered_set<string> highway_types_;
    unordered_map<xml_id, vertex_id> loaded_verts_;
    size_t oneways_;
    size_t revoved_edges_;
};

osm_loader::osm_loader(const string& path, vis_graph *pgraph)
: pgraph_(pgraph)
, mins(std::numeric_limits<vis_coord::value_type>::min(), std::numeric_limits<vis_coord::value_type>::min())
, maxs(std::numeric_limits<vis_coord::value_type>::max(), std::numeric_limits<vis_coord::value_type>::max())
, vert_counter_(0)
, oneways_(0)
, revoved_edges_(0)
{
    ifstream src;
    src.exceptions(ifstream::failbit | ifstream::badbit);
    src.open(path.c_str());

    src.seekg(0, std::ios::end);
    size_t size = src.tellg();
    src.seekg(0);

    cout << "Loading " << path << endl;
    text_.reset(new char[size+1]);
    src.read(text_.get(), size);
    text_[size] = '\0';

    cout << "Parsing " << path << endl;
    doc_.parse<0>(text_.get());

    pgraph_->clear();
    load_bounds();
}

void osm_loader::load_bounds() 
{
    xml_node *root = doc_.first_node("osm");
    if (root == NULL)
        return;

    string str = root->first_node("bound")->first_attribute("box")->value();
    vector<string> strs;
    boost::split(strs, str, boost::is_any_of(","));

    mins.x = atof(strs[0].c_str());
    mins.y = atof(strs[1].c_str());
    maxs.x = atof(strs[2].c_str());
    maxs.y = atof(strs[3].c_str());
}


void osm_loader::load_verts() 
{
    xml_node *root = doc_.first_node("osm");
    if (root == NULL)
        return;
    
    size_t counter = 0;
    xml_node *node = root->first_node("node");
    while (node != NULL)
    {
        if (counter >= MAX_VERTS)
            break;
        xml_id id = atol(node->first_attribute("id")->value());

        if (pgraph_->vertex_exists(id))
        {
            vis_coord coord (atof(node->first_attribute("lat")->value()),
                             atof(node->first_attribute("lon")->value()));

            
            vis_vertex &v = pgraph_->get_vertex(id);
            vis_vertex_data &ref_data = v.get_data();

            ref_data.c = coord;
            
            int degree = v.get_out_degree();
            if (degree >= degrees_stats_.size())
                degrees_stats_.resize(degree + 1, 0);
            ++degrees_stats_[degree];
            

            if (counter % 100000 == 0)
                cout << counter << " verts loaded" << endl;
            ++counter;
        }
        node = node->next_sibling("node");
    }

    update_edge_weights();

}

void osm_loader::prepare_verts(unordered_set<xml_id> &verts_pending) 
{
    cout << "Preparing verts" << endl;
    
    xml_node *root = doc_.first_node("osm");
    if (root == NULL)
        return;

    size_t counter = 0;
    xml_node *node = root->first_node("node");
    while (node != NULL)
    {
        xml_id id = atol(node->first_attribute("id")->value());
        verts_pending.insert(id);
        node = node->next_sibling("node");
    }
}

void osm_loader::load_edges()
{
    unordered_set<xml_id> verts_pending;
    prepare_verts(verts_pending);

    cout << "Loading edges" << endl;

    xml_node *root = doc_.first_node("osm");
    if (root == NULL)
        return;

    xml_node *node = root->first_node("way");
    while (node != NULL)
    {
        if (pgraph_->v_count() >= MAX_VERTS)
            break;

        load_way (node, verts_pending);
        node = node->next_sibling("way");
    }

}
void osm_loader::load_way(xml_node *root, unordered_set<xml_id> &verts_pending)
{
    if (!check_if_highway(root))
        return;

    bool oneway = check_if_oneway(root);
    
    xml_node *current = root->first_node("nd");
    xml_node *next = current->next_sibling("nd");
    size_t counter = 0;
    while (next != NULL)
    {
        load_edge(current, next, oneway, verts_pending);
        current = next;
        next = next->next_sibling("nd");
        ++counter;
    }

    if (way_stats_.size() <= counter)
        way_stats_.resize(counter+1, 0);
    ++way_stats_[counter];
   
}


void osm_loader::load_edge(xml_node *node1, xml_node *node2, bool oneway, unordered_set<xml_id> &verts_pending)
{
    if (oneway)
        ++oneways_;
    static my_graph::edge_weight max_len = 0;

    xml_id vid1 = atol(node1->first_attribute("ref")->value());
    xml_id vid2 = atol(node2->first_attribute("ref")->value());

    if (verts_pending.count(vid1) == 0 || verts_pending.count(vid2) == 0)
    {
        ++revoved_edges_;
        return;
    }

    const vis_vertex &v1 = lazy_get_vertex(vid1);
    const vis_vertex &v2 = lazy_get_vertex(vid2);

    /*coord<double> c = v1.get_data().c - v2.get_data().c;
    my_graph::edge_weight len = static_cast<my_graph::edge_weight>(::sqrt (c.x*c.x + c.y*c.y) / SCALE);

    if (len > max_len)
    {
        cout << "max_len: " << len << endl;
        max_len = len;
    }*/

    vis_edge_data data (0);

    pgraph_->add_edge(vid1, vid2, data);
    if (!oneway)
        pgraph_->add_edge(vid2, vid1, data);

    if (pgraph_->e_count() % 100000 == 0)
        cout << pgraph_->e_count() << " edges loaded" << endl;
}

vis_vertex& osm_loader::lazy_get_vertex(xml_id id)
{
    if (loaded_verts_.count())
    
    if (pgraph_->vertex_exists(id))
        return pgraph_->get_vertex(id);

    ++vert_counter_;
    
    pgraph_->add_vertex(id, vis_vertex_data(0, 0));
    return pgraph_->get_vertex(id);
}

bool osm_loader::check_if_highway(xml_node *root)
{
    xml_node *tag = root->first_node("tag");
    while (tag != NULL)
    {
        if (strcmp(tag->first_attribute("k")->value(), "highway") == 0)
        {
            highway_types_.insert (tag->first_attribute("v")->value());
            return true;
        }
        tag = tag->next_sibling("tag");
    }
    return false;
}

bool osm_loader::check_if_oneway(xml_node *root)
{
    xml_node *tag = root->first_node("tag");
    while (tag != NULL)
    {
        if (strcmp(tag->first_attribute("k")->value(), "oneway") == 0)
        {
            if (strcmp(tag->first_attribute("v")->value(), "yes") == 0)
                return true;
            return false;
        }
        tag = tag->next_sibling("tag");
    }
    return false;
}

void osm_loader::update_edge_weights()
{
    for (vis_graph::e_iterator it = pgraph_->e_begin(); it != pgraph_->e_end(); ++it)
    {
        vis_edge_data &ref_data = it->second.get_data();
        coord<double> c = it->second.get_v1().get_data().c - it->second.get_v2().get_data().c;
        ref_data.len_ = static_cast<my_graph::edge_weight>(::sqrt (c.x*c.x + c.y*c.y));

    }
}

void osm_loader::print_stats()
{
    cout << "Verts: " << pgraph_->v_count() << endl;
    cout << "Edges: " << pgraph_->e_count() << endl;
    
    /*int total = 0;
    cout << "Way stats:" << endl;
    for (size_t i = 0; i < way_stats_.size(); ++i)
    {
        if (way_stats_[i] != 0)
            cout << " " << i << ": " << way_stats_[i] << endl;
        total += way_stats_[i];
    }
    cout << "Total: " << total << endl;*/
    /*cout << "Highway types: ";
    for (unordered_set<string>::const_iterator it = highway_types_.begin();
                                               it != highway_types_.end(); ++it)
    {
        cout << " " << *it << endl;
    }*/

    cout << "Vert degrees: " << endl;
    for (size_t i = 0; i < degrees_stats_.size(); ++i)
        cout << " " << i << ": " << degrees_stats_[i] << endl;

    cout << "Oneways: " << oneways_ << endl;
    cout << "Removed edges: " << revoved_edges_ << endl;
}


void load_osm(const string &path, vis_graph &ref_graph, vis_coord &ref_mins, vis_coord &ref_maxs)
{
    osm_loader loader (path, &ref_graph);
    loader.load_edges();
    loader.load_verts();
    loader.print_stats();
    ref_mins = loader.get_mins();
    ref_maxs = loader.get_maxs();
}