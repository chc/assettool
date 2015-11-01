#include "CCHCMaxXML.h"
#include "CMaterial.h"
#include "CMesh.h"
#include "ScenePack.h"
#include <gd.h>
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
enum EOutputSignature {
	EOSig_HSL,
	EOSig_Bool,
	EOSig_Float,
	EOSig_Int,
	EOSig_Texture,
};

struct materialOutput {
	const char *name;
	const char *attribute;
	EOutputSignature signature;
	union {
		void (CMaterial::*mpFuncHSL)(float r, float g, float b, float a);
		void (CMaterial::*mpFuncBool)(bool v);
		void (CMaterial::*mpFuncFloat)(float s);
		void (CMaterial::*mpFuncInt)(int s);
		void (CMaterial::*mpFuncTexture)(CTexture *tex, int level);
	};

} materialOutputs[] = {
	{"specular_colour", NULL, EOSig_HSL, &CMaterial::setSpecColour},
	{"ambient_colour", NULL,EOSig_HSL, (void (CMaterial::*)(float, float, float, float))&CMaterial::setAmbientColour},
	//{"diffuse_colour", NULL,EOSig_HSL, NULL},
	{"shine", "shine",EOSig_Float, (void (CMaterial::*)(float, float, float, float))&CMaterial::setShine},
	{"shine", "shine_strength",EOSig_Float, (void (CMaterial::*)(float, float, float, float))&CMaterial::setShineStrength},
	//{"transparency", "transparency",EOSig_Bool, NULL},
	//{"twosided", "value",EOSig_Bool, NULL},
	//{"wireframe", "value",EOSig_Bool, NULL},
	{"texture", NULL,EOSig_Texture, (void (CMaterial::*)(float, float, float, float))&CMaterial::setTexture},
};
materialOutput *getMaterialOutput(const char *name, const char *attr) {
	for(int i=0;i<sizeof(materialOutputs)/sizeof(materialOutput);i++) {
		if(strcmp(name,materialOutputs[i].name) == 0) {
			if(attr != NULL) {
				if(strcmp(attr,materialOutputs[i].attribute) == 0) {
					return &materialOutputs[i];
				}
			} else {
				return &materialOutputs[i];
			}
		}
	}
	return NULL;
}
CTexture *load_texture(const char *path, bool tile_u, bool tile_v, float u_offset, float v_offset) {
	FILE *fd = fopen(path,"rb");
	gdImagePtr img =  gdImageCreateFromPng(fd);
	if(img) {
		CTexture *tex = new CTexture();
		tex->setDimensions(img->sx, img->sy);

		//TODO: free this somewhere
		uint32_t *img_data = (uint32_t *)malloc(img->sx*img->sy*sizeof(uint32_t));
		int i =0;
		for(int x = 0;x<img->sx;x++) {
			for(int y = 0;y<img->sy;y++) {
				img_data[i++] = gdImageGetPixel(img,x,y);
			}
		}	
		tex->setColourData(EColourType_32BPP, img_data, img->sx*img->sy*sizeof(uint32_t), true);
		return tex;
	}
	return NULL;
}
void load_material_data(pugi::xml_node node, CMaterial *material) {
	pugi::xml_attribute name_attr = node.attribute("name");
	if(!name_attr.empty()) {
		material->setName(name_attr.as_string());
	}
	for (pugi::xml_node tool = node.first_child(); tool; tool = tool.next_sibling()) {
		std::cout << tool.name() << std::endl; 
		materialOutput *output = getMaterialOutput(tool.name(), NULL);
		if(output != NULL) {
			switch(output->signature) {
			case EOSig_HSL:
				((*material).*(output->mpFuncHSL))(tool.attribute("r").as_float(),tool.attribute("g").as_float(),tool.attribute("b").as_float(),tool.attribute("a").as_float());
				break;
			case EOSig_Bool:
				((*material).*(output->mpFuncBool))(tool.attribute(output->attribute).as_bool());
				break;
			case EOSig_Float:
				((*material).*(output->mpFuncFloat))(tool.attribute(output->attribute).as_float());
				break;
			case EOSig_Texture: 
				//gdImageCreateFromFile
				((*material).*(output->mpFuncTexture))(load_texture(tool.attribute("path").as_string(),tool.attribute("tile_u").as_bool(),tool.attribute("tile_v").as_bool(),tool.attribute("u_offset").as_float(),tool.attribute("v_offset").as_float()),0);
				//CTexture *tex = new CTexture();
				break;
			}
		}
	}
}
bool chc_max_xml_import(ImportOptions *impOpts) {
	pugi::xml_document doc;

	//load mesh data
	char name[FILENAME_MAX+1];
	sprintf(name,"%s.mesh.xml",impOpts->path);
	doc.load_file(name);
	int num_meshes = std::distance(doc.begin(),doc.end());
	int i =0;
	CMesh **meshes = (CMesh**)malloc(num_meshes*sizeof(CMesh*));
	pugi::xml_node xmeshes = doc.child("mesh");
	for (pugi::xml_node tool = xmeshes.first_child(); tool; tool = tool.next_sibling()) {
		int size = std::distance(tool.children().begin(),tool.children().end());
		std::cout << tool.name() << " " << size<< std::endl; 
		meshes[i] = new CMesh();
		load_mesh_data(tool,meshes[i]);
		i++;
	}

	//load material data
	sprintf(name,"%s.mat.xml",impOpts->path);
	doc.load_file(name);
	pugi::xml_node::iterator it = doc.begin();
	int num_materials = std::distance(doc.begin(),doc.end());
	CMaterial **materials = (CMaterial**)malloc(num_materials*sizeof(CMaterial*));
	i = 0;
	while(it != doc.end()) {
		pugi::xml_node node = *it;
		std::cout << node.name() << " " << num_materials << std::endl; 
		materials[i] = new CMaterial();
		load_material_data(node,materials[i++]);
		it++;
	}

	//run exporter
	ScenePack scene;
	memset(&scene,0,sizeof(scene));
	scene.num_meshes = num_meshes;
	scene.num_materials = num_materials;


	//free resources
	for(int i=0;i<num_materials;i++) {
		if(materials[i] != NULL) {
			delete materials[i];
		}
	}
	free(materials);
	return false;
}
bool chc_max_xml_export(ExportOptions *expOpts) {
	return false;
}
