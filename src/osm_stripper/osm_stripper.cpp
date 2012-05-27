// osm_stripper.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

typedef rapidxml::xml_document<> xml_document;
typedef rapidxml::xml_node<> xml_node;

using std::ifstream;
using std::cout;
using std::endl;



int main(int argc, char* argv[])
{
    char filename[] = "c:/d/RU-SPE.osm";
    boost::scoped_array<char> text;
    
    ifstream src (filename);

    src.seekg(0, std::ios::end);
    size_t size = src.tellg();
    src.seekg(0);

    cout << "Loading " << filename << endl;
    text.reset(new char[size+1]);
    src.read(text.get(), size);
    text[size] = '\0';

    cout << "Parsing " << filename << endl;
    xml_document doc;
    doc.parse<0>(text.get());
	return 0;
}

