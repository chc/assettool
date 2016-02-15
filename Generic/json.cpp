#include "json.h"
#include <jansson.h>
#include <Generic/ScenePack.h>
bool gen_import_json_mesh(ImportOptions* opts) {
	return false;
}
void add_mesh_to_json(json_t *jobj, CMesh *mesh) {


	json_t* jarr1 = json_array();
	int num_verts = mesh->getNumVertices();
	float *p = mesh->getVerticies();
	json_t *material_checksum = 0;
	json_t *name = json_string(mesh->getName());
	json_object_set_new(jobj, "name", name);
	printf("associated mat: %p\n", mesh->getMaterial());
	if (mesh->getMaterial()) {
		material_checksum = json_integer(mesh->getMaterial()->getIdentifierChecksum());
		json_object_set_new(jobj, "material_checksum", material_checksum);
	}
	
	//add verts to json
	for (int i = 0; i < num_verts; i++) {
		json_t* v_array = json_array();
		for (int j = 0; j < 3; j++) {
			double v = (*p);
			json_t* jval = json_real(v);
			json_array_append_new(v_array, jval);
			p++;
		}
		p++; //skip W
		json_array_append_new(jarr1, v_array);
	}
	json_object_set_new(jobj, "vertices", jarr1);

	jarr1 = json_array();
	//normals
	p = mesh->getNormals();
	if (p) {
		for (int i = 0; i < num_verts; i++) {
			json_t* v_array = json_array();
			for (int j = 0; j < 3; j++) {
				double v = (*p);
				json_t* jval = json_real(v);
				json_array_append_new(v_array, jval);
				p++;
			}
			p++; //skip W
			json_array_append_new(jarr1, v_array);
		}
		json_object_set_new(jobj, "normals", jarr1);
	}

	jarr1 = json_array();
	
	//uvs
	json_t *uv_array = json_array();
	if (mesh->getUVWs(0) != NULL) {
		for (int l = 0; l < mesh->getUVLayers() || l == 0; l++) {
			json_t *uv_data_array = json_array();
			p = mesh->getUVWs(l);
			for (int i = 0; i < num_verts; i++) {
				json_t* v_array = json_array();
				for (int j = 0; j < 3; j++) {
					double v = (*p);
					json_t* jval = json_real(v);
					json_array_append_new(v_array, jval);
					p++;
				}
				p++; //skip W
				json_array_append_new(uv_array, v_array);
			}
		}
		json_object_set_new(jobj, "uvs", uv_array);
	}


	//dump indices
	int num_index_sets = mesh->getNumIndexLevels();
	if (mesh->getIndices() != NULL)
	{
		json_t *indices_array2 = json_array();
		bool sub_indices = mesh->hasSubIndices();
		for (int i = 0; i < num_index_sets || i == 0; i++) {
			int level = i;
			uint32_t *indices = mesh->getIndices(level);
			if (!sub_indices) {
				level = -1;
				indices = mesh->getIndices(level);
			}
			json_t *indices_array = json_array();
			uint32_t num_indices = mesh->getNumIndicies(level);
			for (int k = 0; k < num_indices / 3; k++) {
				//TODO: make indices count based off primitive type
				json_t* v_array = json_array();
				for(int j=0;j<3;j++) {
					int index = *indices;
					json_t* jval = json_integer(index);
					indices++;
					json_array_append_new(v_array, jval);
				}
				json_array_append_new(indices_array, v_array);
				//int index = *indices;
				//json_t* jval = json_integer(index);
				//json_array_append_new(indices_array, jval);
				//indices++;
			}
			json_array_append_new(indices_array2, indices_array);
		}
		if (sub_indices) {
			json_object_set_new(jobj, "submesh_materials", json_integer(1));
		}
		json_object_set_new(jobj, "indices", indices_array2);
	}	
}
void add_model_skeleton_to_json(json_t *jobj, CMesh *mesh) {
	int num_bones = mesh->getNumBones();

	json_t* skel_obj = json_object();
	json_t *bones = json_array();

	for(int i=0;i<num_bones;i++) {

		json_t* bone_obj = json_object();

		sBone *bone = mesh->getBone(i);

		float *matrix = (float *)&bone->matrix;
		float *mat = matrix;

		const char *name = (const char *)bone->identifier.sUnion.mString;

		json_object_set_new(bone_obj, "name", json_string(name));
		
		if(bone->parent) {
			const char *parent_name = bone->parent->identifier.sUnion.mString;
			json_object_set_new(bone_obj, "parent", json_string(parent_name));
		}
		json_t* bone_matrix = json_array();
		for(int j=0;j<4;j++) {
			json_t *element_array = json_array();
			for(int k=0;k<4;k++) {
				json_array_append_new(element_array, json_real(*mat));	
				mat++;
			}
			json_array_append_new(bone_matrix, element_array);
		}
		json_object_set_new(bone_obj, "matrix", bone_matrix);

		json_array_append_new(bones, bone_obj);
	}
	json_object_set_new(jobj, "skeleton", bones);
	
}
void add_material_to_json(json_t *jobj, CMaterial *mat) {
	CTexture *tex;
	json_t *mat_obj = json_object();
	printf("adding mat: %p\n", mat);
	json_t *checksum = json_integer(mat->getIdentifierChecksum());
	json_t *name = json_string(mat->getName());
	json_object_set_new(mat_obj, "checksum", checksum);
	json_object_set_new(mat_obj, "name", name);
	bool wrote_materials = false;
	json_t *textures = json_array();
	for (int i = 0; (tex = mat->getTexture(i)) != NULL; i++) {
		json_t *tex_obj = json_object();
		const char *file_name = strrchr(tex->getPath(), '\\');
		json_t *level = json_integer(i);
		json_object_set_new(tex_obj, "level", level);
		if (file_name) {
			file_name++;
			json_t *str = json_string(file_name);
			json_object_set_new(tex_obj, "path", str);
		}
		json_array_append_new(textures, tex_obj);
		wrote_materials = true;
	}
	if (!wrote_materials) {
		for (int i = 0; i < MAX_MATERIAL_TEXTURES; i++) {
			json_t *tex_obj = json_object();
			const char *name = mat->getTextureName(i);
			json_t *level;
			if(strlen(name) > 0) {
				level = json_string(name);
				json_object_set_new(tex_obj, "name", level);
			} else {
				uint32_t checksum = mat->getTextureChecksum(i);
				if (checksum == 0) break;
				level = json_integer(checksum);				
				json_object_set_new(tex_obj, "checksum", level);
			}

			
			json_array_append_new(textures, tex_obj);
		}
		json_object_set_new(mat_obj, "textures", textures);
	}
	json_array_append_new(jobj, mat_obj);
}
bool gen_export_json_mesh(ExportOptions* opts) {
	ScenePack *pack = (ScenePack *)opts->dataClass;
	json_t *json_file = json_object();
	json_t *meshes = json_array();
	for (int i = 0; i < pack->num_meshes; i++) {
		json_t *mesh_data = json_object();
		add_mesh_to_json(mesh_data, pack->m_meshes[i]);
		json_array_append_new(meshes, mesh_data);

		add_model_skeleton_to_json(mesh_data, pack->m_meshes[i]);
	}
	json_object_set_new(json_file, "meshes", meshes);
	json_t *material_array = json_array();
	for (int i = 0; i < pack->num_materials; i++) {
		add_material_to_json(material_array, pack->m_materials[i]);
	}
	json_object_set_new(json_file, "materials", material_array);
	char *s = json_dumps(json_file, JSON_INDENT(1));
	FILE *fd = fopen(opts->path, "w");
	fwrite(s, strlen(s), 1, fd);
	fclose(fd);
	return false;
}