#include "CCHCMaxXML.h"
#include "CMaterial.h"
#include "CMesh.h"
#include "ScenePack.h"
//#include <gd.h>
#include <png.h>
#include <iostream>
#include <pugixml.hpp>
#include <iterator>
#include "crc32.h"
#include "CCollision.h"
#include <fstream>
#include "Map.h"
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
		std::cout << "Index count: " << size << std::endl;
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
std::vector<void (CMaterial::*)()> funcs;
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
	static int cnt = 0;
	static Core::Map<int, CTexture *> loaded_textures;
	int checksum = crc32(0, path, strlen(path));
	if(loaded_textures[checksum] != NULL) {
		return loaded_textures[checksum];
	}

	FILE *fd = fopen(path,"rb");
	if(!fd) return NULL;
	CImage *tex = new CImage();
	png_uint_32 width, height;
	int bit_depth, color_type;

	png_structp png_ptr;
	png_infop info_ptr;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,NULL);

	info_ptr = png_create_info_struct(png_ptr);	
	png_init_io(png_ptr, fd);
	png_byte color;
	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
		&color_type, NULL, NULL, NULL);

	/* tell libpng to strip 16 bit/color files down to 8 bits/color */
	png_set_strip_16(png_ptr);

	/* Strip alpha bytes from the input data without combining with the
	* background (not recommended).
	*/
	//png_set_strip_alpha(png_ptr);

	/* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
	* byte into separate bytes (useful for paletted and grayscale images).
	*/
	png_set_packing(png_ptr);

	/* Change the order of packed pixels to least significant bit first
	* (not useful if you are using png_set_packing). */
	//png_set_packswap(png_ptr);

	png_set_expand(png_ptr);

	/* Set the background color to draw transparent and alpha images over.
	* It is possible to set the red, green, and blue components directly
	* for paletted images instead of supplying a palette index.  Note that
	* even if the PNG file supplies a background, you are not required to
	* use it - you should use the (solid) application background if it has one.
	*/

	png_color_16 my_background, *image_background;

	if (png_get_bKGD(png_ptr, info_ptr, &image_background))
		png_set_background(png_ptr, image_background,
							PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);

	/* invert monochrome files to have 0 as white and 1 as black */
	png_set_invert_mono(png_ptr);

	/* If you want to shift the pixel values from the range [0,255] or
	* [0,65535] to the original [0,7] or [0,31], or whatever range the
	* colors were originally in:
	*/
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_sBIT))
	{
		png_color_8p sig_bit;

		png_get_sBIT(png_ptr, info_ptr, &sig_bit);
		png_set_shift(png_ptr, sig_bit);
	}

	/* swap the RGBA or GA data to ARGB or AG (or BGRA to ABGR) */
	//png_set_swap_alpha(png_ptr);

	/* swap bytes of 16 bit files to least significant byte first */
	png_set_swap(png_ptr);

	/* Add filler (or alpha) byte (before/after each RGB triplet) */
	png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

	/* Turn on interlace handling.  REQUIRED if you are not using
	* png_read_image().  To see how to handle interlacing passes,
	* see the png_read_row() method below:
	*/
	int number_passes = png_set_interlace_handling(png_ptr);

	png_read_update_info(png_ptr,info_ptr);

	tex->setDimensions(width,height);

	int num_palette = 256, num_trans = 256;
	png_color palette[256];
	png_byte transparency[256];
	png_bytepp row_pointers;

	row_pointers = (png_bytepp)png_malloc(png_ptr, height * sizeof(png_bytep));

	for (int row = 0; row < height; row++)
	{
		row_pointers[row] = (png_bytep)png_malloc(png_ptr, png_get_rowbytes(png_ptr,info_ptr));
	}
	png_read_image(png_ptr,row_pointers);


	//make this for all colour types
	void *col_data = malloc(sizeof(uint32_t) * width * height);
	char *p = (char *)col_data;
	int len = png_get_rowbytes(png_ptr,info_ptr);
	for(int i=0;i<height;i++) {
		memcpy(p,row_pointers[i],len);
		p += len;
	}

	tex->setColourData(EColourType_32BPP,col_data, sizeof(uint32_t) * width * height);


	for (int row = 0; row < height; row++)
	{
		png_free(png_ptr, row_pointers[row]);
	}
	png_free(png_ptr, row_pointers);

	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	/* close the file */
	fclose(fd);


	CTexture *real_tex = new CTexture();

	real_tex->setChecksum(crc32(0,path,strlen(path)));
	real_tex->setUVOffset(u_offset, v_offset);
	real_tex->setUVTiling(tile_u, tile_v);
	real_tex->setImage(tex);
	real_tex->setPath(path);

	loaded_textures[checksum] = real_tex;
	return real_tex;
}

void load_material_data(pugi::xml_node node, CMaterial *material) {
	pugi::xml_attribute name_attr = node.attribute("name");
	if(!name_attr.empty()) {
		material->setName(name_attr.as_string());
	}
	for (pugi::xml_node tool = node.first_child(); tool; tool = tool.next_sibling()) {
		//std::cout << tool.name() << std::endl; 
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
int count_nodes_named(const char *name, pugi::xml_document *doc, pugi::xml_node *node) {
	int count = 0;
	if(doc) {
		pugi::xml_node::iterator it = doc->begin();
		while(it != doc->end()) {
			pugi::xml_node child = *it;
			if(strcmp(child.name(),name) == 0) count++;
			count += count_nodes_named(name,NULL,&child);
			it++;
		}
	} else {
		for (pugi::xml_node mesh_data = node->first_child(); mesh_data; mesh_data = mesh_data.next_sibling()) {
			if(strcmp(mesh_data.name(),name) == 0) {
				count++;
			}
		}
	}
	return count;
}

void chc_mesh_do_import(pugi::xml_node *child, CMesh *out_mesh, CMaterial **materials, uint32_t num_materials, uint32_t group_id) {
	CMaterial *mat = find_material_by_name(materials,num_materials,child->attribute("material_name").as_string());		
	(out_mesh)->setMaterial(mat);
	out_mesh->setGroupId(group_id);
	for (pugi::xml_node tool = child->first_child(); tool; tool = tool.next_sibling()) {
		load_mesh_data(tool,out_mesh);
	}
}
void chc_max_import_meshes(CMesh **meshes, pugi::xml_document *doc, pugi::xml_node *node, CMaterial **materials, uint32_t num_materials, uint32_t group_id = 0) {
	const char *name = "mesh";
	static int idx = -1;
	if(doc) idx = -1;
	idx++;
	if(doc) {
		pugi::xml_node::iterator it = doc->begin();
		while(it != doc->end()) {
			pugi::xml_node child = *it;
			const char *name = child.name();
			if(!strcmp(name,"group")) {			
				const char *group_name = child.attribute("name").as_string();
				uint32_t group_checksum = crc32(0,group_name,strlen(group_name));
				for (pugi::xml_node tool = child.first_child(); tool; tool = tool.next_sibling()) {
					chc_max_import_meshes(&meshes[idx],NULL,&tool,materials,num_materials, group_checksum);
				}
			} else {
				chc_max_import_meshes(&meshes[idx],NULL,&child,materials,num_materials, 0);
			}
			it++;
		}
	} else {
		chc_mesh_do_import(node,*meshes,materials,num_materials, group_id);
	}
}
void chc_max_import_collision(CCollision *collision, pugi::xml_document *doc) {
	for (pugi::xml_node tool = doc->first_child(); tool; tool = tool.next_sibling()) {
		if(strcmp(tool.attribute("type").as_string(), "bbox") == 0) {
			BBox box;
			memset(&box,0, sizeof(box));
			box.checksum = crc32(0, tool.attribute("name").as_string(), strlen(tool.attribute("name").as_string()));
			pugi::xml_node bounds_node = tool.child("bounds");

			box.min[0] = bounds_node.attribute("minx").as_float();
			box.min[1] = bounds_node.attribute("miny").as_float();
			box.min[2] = bounds_node.attribute("minz").as_float();

			box.max[0] = bounds_node.attribute("maxx").as_float();
			box.max[1] = bounds_node.attribute("maxy").as_float();
			box.max[2] = bounds_node.attribute("maxz").as_float();
			collision->addBBOX(box);
		}
	}
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
		//std::cout << node.name() << " " << num_materials << std::endl; 
		materials[i] = new CMaterial();
		load_material_data(node,materials[i++]);
		it++;
	}

	//load mesh data
	sprintf(name,"%s.mesh.xml",impOpts->path);
	doc.load_file(name);
	doc.child("mesh");
	int num_meshes = count_nodes_named("mesh",&doc,NULL);


	CMesh **meshes = (CMesh**)malloc(num_meshes*sizeof(CMesh*));
	memset(meshes,0,sizeof(CMesh*)*num_meshes);
	for(int i=0;i<num_meshes;i++) {
		meshes[i] = new CMesh();
	}
	chc_max_import_meshes(meshes, &doc, NULL, materials, num_materials);


	//load collision data
	sprintf(name,"%s.col.xml",impOpts->path);
	doc.load_file(name);
	doc.child("mesh");

	int num_colobjs = count_nodes_named("mesh",&doc,NULL);


	CCollision *collision = new CCollision();
	chc_max_import_collision(collision, &doc);



	//run exporter
	ScenePack scene;
	memset(&scene,0,sizeof(scene));
	scene.m_meshes = (CMesh**)meshes;
	scene.m_materials = (CMaterial**)materials;
	scene.num_meshes = num_meshes;
	scene.num_materials = num_materials;
	scene.m_collision = collision;
	
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
void export_xml_mesh(pugi::xml_document *m_mesh_xml, CMesh *mesh) {
	pugi::xml_node main_node = m_mesh_xml->append_child();
	main_node.set_name("mesh");
	main_node.append_attribute("name") = mesh->getName();

	pugi::xml_node xnode = main_node.append_child();
	xnode.set_name("default_transforms");
	float *pos = mesh->getDefaultHierarchialPosition();
	xnode.append_attribute("position_x") = *pos++;
	xnode.append_attribute("position_y") = *pos++;
	xnode.append_attribute("position_z") = *pos++;

	xnode = main_node.append_child();
    xnode.set_name("verticies");
	int num_verts = mesh->getNumVertices();
	float *verts = mesh->getVerticies();
	float *p = verts;
	for (int i=0; i<num_verts; i++) {
		pugi::xml_node param = xnode.append_child();
		param.set_name("point");

		// add attributes to param node
		param.append_attribute("x") = *p++;
		param.append_attribute("y") = *p++;
		param.append_attribute("z") = *p++;
	}


	float *normals = mesh->getNormals();
	p = normals;
	if(normals) {
		xnode = main_node.append_child();
		xnode.set_name("normals");
		for (int i=0; i<num_verts; i++) {
			pugi::xml_node param = xnode.append_child();
			param.set_name("point");

			// add attributes to param node
			param.append_attribute("x") = *p++;
			param.append_attribute("y") = *p++;
			param.append_attribute("z") = *p++;
		}
	}

	int num_uv_sets = mesh->getUVLayers();
	if(num_uv_sets > 0) {
		xnode = main_node.append_child();
		xnode.set_name("uvs");	
		
		for(int i=0;i<num_uv_sets;i++) {
			pugi::xml_node uv_node = xnode.append_child();
			uv_node.set_name("set"); //idk what to call it
			uv_node.append_attribute("layer") = i;
			float *uvs = mesh->getUVWs(i);
			p = uvs;
			for(int j=0;j<num_verts;j++) {
				pugi::xml_node param = uv_node.append_child();
				param.set_name("point");
				param.append_attribute("u") = *p++;
				param.append_attribute("v") = *p++;
			}
		}
	}

	xnode = main_node.append_child();
	xnode.set_name("indices");	


	int num_index_levels = mesh->getNumIndexLevels();
	
	for(int i=0;i<num_index_levels;i++) {
			uint32_t *index = mesh->getIndices(i);
			pugi::xml_node index_node = xnode.append_child();
			index_node.set_name("set"); //idk what to call it
			index_node.append_attribute("layer") = i;
			for(int j=0;j<mesh->getNumIndicies(i);j++) {
				pugi::xml_node param = index_node.append_child();
				param.set_name("point");
				param.append_attribute("x") = *index++;
				param.append_attribute("y") = *index++;
				param.append_attribute("z") = *index++;
			}
	}

	
}
void export_xml_material(pugi::xml_document *m_mat_xml, CMaterial *mat) {
	pugi::xml_node main_node = m_mat_xml->append_child();
	main_node.set_name("material");
	uint64_t flags = mat->getFlags();

	main_node.append_attribute("name") = mat->getName();

	if(flags & EMaterialFlag_HasAmbientIntensitiy)
		main_node.append_attribute("ambient_reflection_coeff") = mat->getAmbientReflectionCoeff();
	if(flags & EMaterialFlag_HasSpecIntensitiy)
		main_node.append_attribute("specular_reflection_coeff") = mat->getSpecularReflectionCoeff();
	if(flags & EMaterialFlag_HasDiffuseIntensitiy)
		main_node.append_attribute("diffuse_reflection_coeff") = mat->getDiffuseReflectionCoeff();

	CTexture *tex = NULL;
	int i = 0;
	while((tex = mat->getTexture(i)) != NULL) {
		pugi::xml_node node = main_node.append_child();
		node.set_name("texture");
		node.append_attribute("path") = tex->getPath();
		/*
				mat->setTextureFilterMode(texrec->mat_filter_mode, level);
		mat->setTextureAddressMode(texrec->u_mode, texrec->v_mode, level++);
		*/

		ETextureAddresingMode u, v;
		mat->getTextureAddressModes(u, v, i);
		node.append_attribute("address_u_mode") = u;
		node.append_attribute("address_v_mode") = v;

		ETextureFilterMode mode = mat->getTextureFilterMode(i);
		node.append_attribute("filter_mode") = mode;
		node.append_attribute("blend_mode") = mat->getBlendMode(i);
		i++;
	}
	
}
bool chc_max_xml_export(ExportOptions *expOpts) {
	pugi::xml_document m_mesh_xml, m_mat_xml;
	char out_name[64];
	sprintf(out_name,"%s.mesh.xml",expOpts->path);
	std::ofstream mesh_xml_out;

	ScenePack *pack = (ScenePack *)expOpts->dataClass;

	for(int i=0;i<pack->num_meshes;i++) {
		export_xml_mesh(&m_mesh_xml, pack->m_meshes[i]);
	}

	mesh_xml_out.open(out_name);
	m_mesh_xml.save(mesh_xml_out);
	mesh_xml_out.close();

	sprintf(out_name,"%s.mat.xml",expOpts->path);
	std::ofstream mat_xml_out;

	for(int i=0;i<pack->num_materials;i++) {
		export_xml_material(&m_mat_xml, pack->m_materials[i]);
	}

	mat_xml_out.open(out_name);
	m_mat_xml.save(mat_xml_out);
	mesh_xml_out.close();

	return false;
}
