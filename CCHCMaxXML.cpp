#include "CCHCMaxXML.h"
#include "CMaterial.h"
#include "CMesh.h"
#include "ScenePack.h"
#include <gd.h>
#include <png.h>
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
	if(!fd) return false;
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
	png_set_filler(png_ptr, 0x22, PNG_FILLER_AFTER);

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

	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	/* close the file */
	fclose(fd);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	

	CTexture *real_tex = new CTexture();

	real_tex->setChecksum(crc32(0,path,strlen(path)));
	real_tex->setUVOffset(u_offset, v_offset);
	real_tex->setUVTiling(tile_u, tile_v);
	real_tex->setImage(tex);
	real_tex->setPath(path);

	loaded_textures.push_back(real_tex);
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
			//std::cout << tool.name() << " " << size<< std::endl; 
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
