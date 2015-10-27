#include "CCHCMaxXML.h"
#include "CMaterial.h"
#include "CMesh.h"
#include <iostream>
#include <pugixml.hpp>
#include <iterator>
void load_mesh_data(pugi::xml_node node, CMesh *mesh) {
	int size = std::distance(node.children().begin(),node.children().end());
	float *vert_data = (float *)malloc(size * sizeof(float) * 3);
	uint32_t *indicies = (uint32_t*)malloc(sizeof(uint32_t) * size * 3);
	float *p = vert_data;
	uint32_t *i = indicies;
	if(mesh->getNumVertices() == 0) {
		mesh->setNumVerts(size);
	}
	if(strcmp(node.name(),"verticies") == 0) {
		for (pugi::xml_node mesh_data = node.first_child(); mesh_data; mesh_data = mesh_data.next_sibling())
		{
			for (pugi::xml_attribute attr = mesh_data.first_attribute(); attr; attr = attr.next_attribute())
			{
				*p++ = atof(attr.value());
			}
		}
		mesh->setVerticies(vert_data);
	} else if(strcmp(node.name(),"normals") == 0) {
		for (pugi::xml_node mesh_data = node.first_child(); mesh_data; mesh_data = mesh_data.next_sibling())
		{
			for (pugi::xml_attribute attr = mesh_data.first_attribute(); attr; attr = attr.next_attribute())
			{
				*p++ = atof(attr.value());
			}
		}
		mesh->setNormals(vert_data);
	} else if(strcmp(node.name(),"uvws") == 0) {
		for (pugi::xml_node mesh_data = node.first_child(); mesh_data; mesh_data = mesh_data.next_sibling())
		{
			for (pugi::xml_attribute attr = mesh_data.first_attribute(); attr; attr = attr.next_attribute())
			{
				*p++ = atof(attr.value());
			}
		}
	}else if(strcmp(node.name(),"indices") == 0) {
		for (pugi::xml_node mesh_data = node.first_child(); mesh_data; mesh_data = mesh_data.next_sibling())
		{
			for (pugi::xml_attribute attr = mesh_data.first_attribute(); attr; attr = attr.next_attribute())
			{
				*i++ = atoi(attr.value());
			}
		}
		mesh->setIndices(indicies,size);
	}

	free(vert_data);
}
bool chc_max_xml_import(ImportOptions *impOpts) {
	pugi::xml_document doc;

	//load mesh data
	char name[FILENAME_MAX+1];
	sprintf(name,"%s.mesh.xml",impOpts->path);
	doc.load_file(name);
	CMesh *mesh = new CMesh();
	pugi::xml_node meshes = doc.child("mesh");
	for (pugi::xml_node tool = meshes.first_child(); tool; tool = tool.next_sibling()) {
		int size = std::distance(tool.children().begin(),tool.children().end());
		std::cout << tool.name() << " " << size<< std::endl; 
		load_mesh_data(tool,mesh);
	}


	sprintf(name,"%s.mat.xml",impOpts->path);
	doc.load_file(name);
	pugi::xml_node::iterator it = doc.begin();
	int num_materials = std::distance(doc.begin(),doc.end());
	while(it != doc.end()) {
		pugi::xml_node node = *it;
		std::cout << node.name() << " " << num_materials << std::endl; 
		it++;
		//load_mesh_data(tool,mesh);
	}

	 //load material data
	return false;
}
bool chc_max_xml_export(ExportOptions *expOpts) {
	return false;
}
