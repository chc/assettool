#include "CCHCMaxXML.h"
#include "CMaterial.h"
#include "CMesh.h"
#include "ScenePack.h"
#include <gd.h>
#include <iostream>
#include <pugixml.hpp>
#include <iterator>
#include "crc32.h"
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
		mesh->setUVWs(vert_data, 0);
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
	free(indicies);
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

	static std::vector<CTexture *> loaded_textures;
	std::vector<CTexture *>::iterator it = loaded_textures.begin();
	while(it != loaded_textures.end()) {
		CTexture *t = *it;
		if(strcmp(path,t->getPath()) == 0) {
			return t;
		}
		it++;
	}
	FILE *fd = fopen(path,"rb");
	gdImagePtr gdImg =  gdImageCreateFromPng(fd);
	if(gdImg) {
		CTexture *tex = new CTexture();
		CImage *img = new CImage();
		img->setDimensions(gdImg->sx, gdImg->sy);

		//TODO: free this somewhere
		uint32_t *img_data = (uint32_t *)malloc(gdImg->sx*gdImg->sy*sizeof(uint32_t));
		int i =0;
		for(int x = 0;x<gdImg->sx;x++) {
			for(int y = 0;y<gdImg->sy;y++) {
				img_data[i++] = gdImageGetPixel(gdImg,x,y);
			}
		}	
		img->setColourData(EColourType_32BPP, img_data, gdImg->sx*gdImg->sy*sizeof(uint32_t), true);
		tex->setChecksum(crc32(0,path,strlen(path)));
		tex->setUVOffset(u_offset, v_offset);
		tex->setUVTiling(tile_u, tile_v);
		tex->setImage(img);
		tex->setPath(path);

		loaded_textures.push_back(tex);
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
CMaterial *find_material_by_name(CMaterial **mats, int num_mats, const char *name) {
	for(int i=0;i<num_mats;i++) {
		if(strcmp(mats[i]->getName(),name) == 0) {
			return mats[i];
		}
	}
	return NULL;
}
bool chc_max_xml_import(ImportOptions *impOpts) {
	pugi::xml_document doc;
	char name[FILENAME_MAX+1];

	//load material data
	sprintf(name,"%s.mat.xml",impOpts->path);
	doc.load_file(name);
	pugi::xml_node::iterator it = doc.begin();
	int num_materials = std::distance(doc.begin(),doc.end());
	CMaterial **materials = (CMaterial**)malloc(num_materials*sizeof(CMaterial*));
	int i =0;
	while(it != doc.end()) {
		pugi::xml_node node = *it;
		std::cout << node.name() << " " << num_materials << std::endl; 
		materials[i] = new CMaterial();
		load_material_data(node,materials[i++]);
		it++;
	}

	//load mesh data
	sprintf(name,"%s.mesh.xml",impOpts->path);
	doc.load_file(name);
	int num_meshes = std::distance(doc.begin(),doc.end());
	CMesh **meshes = (CMesh**)malloc(num_meshes*sizeof(CMesh*));
	i = 0;
	it = doc.begin();
	while(it != doc.end()) {
		pugi::xml_node xmeshes = *it;
		CMaterial *mat = find_material_by_name(materials,num_materials,xmeshes.attribute("material_name").as_string());		
		meshes[i] = new CMesh();
		meshes[i]->setMaterial(mat);
		for (pugi::xml_node tool = xmeshes.first_child(); tool; tool = tool.next_sibling()) {
			int size = std::distance(tool.children().begin(),tool.children().end());
			std::cout << tool.name() << " " << size<< std::endl; 
			load_mesh_data(tool,meshes[i]);
			
		}
		i++;
		it++;
	}

	//run exporter
	ScenePack scene;
	memset(&scene,0,sizeof(scene));
	scene.m_meshes = (CMesh**)meshes;
	scene.m_materials = (CMaterial**)materials;
	scene.num_meshes = num_meshes;
	scene.num_materials = num_materials;
	
	ExportOptions opts;
	memset(&opts,0,sizeof(opts));


	opts.dataClass = &scene;
	opts.srcPath = impOpts->path;
	opts.args = impOpts->expArgs;
	opts.path = impOpts->outpath;
	impOpts->exporter(&opts);


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
