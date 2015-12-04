#include "json.h"
#include <jansson.h>
#include "ScenePack.h"
bool gen_import_json_mesh(ImportOptions* opts) {
	return false;
}
void add_mesh_to_json(json_t *jobj, CMesh *mesh) {


	json_t* jarr1 = json_array();
	int num_verts = mesh->getNumVertices();
	float *p = mesh->getVerticies();

	//add verts to json
	for (int i = 0; i < num_verts; i++) {
		json_t* v_array = json_array();
		for (int j = 0; j < 3; j++) {
			double v = (*p);
			json_t* jval = json_real(v);
			json_array_append_new(v_array, jval);
			p++;
		}
		json_array_append_new(jarr1, v_array);
	}
	json_object_set_new(jobj, "verticies", jarr1);

	jarr1 = json_array();
	//normals
	p = mesh->getNormals();
	for (int i = 0; i < num_verts; i++) {
		json_t* v_array = json_array();
		for (int j = 0; j < 3; j++) {
			double v = (*p);
			json_t* jval = json_real(v);
			json_array_append_new(v_array, jval);
			p++;
		}
		json_array_append_new(jarr1, v_array);
	}
	json_object_set_new(jobj, "normals", jarr1);

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
		for (int i = 0; i < num_index_sets || i == 0; i++) {
			int level = i;
			uint32_t *indices = mesh->getIndices(level);
			if (indices == NULL && i == 0) {
				level = -1;
				indices = mesh->getIndices(level);
			}
			json_t *indices_array = json_array();
			uint32_t num_indices = mesh->getNumIndicies(level);
			for (int k = 0; k < num_indices; k++) {
				for (int j = 0; j < 3; j++) {
					int index = *indices;
					json_t* jval = json_integer(index);
					json_array_append_new(indices_array, jval);
					indices++;
				}
			}
			json_array_append_new(indices_array2, indices_array);
		}
		json_object_set_new(jobj, "indices", indices_array2);
	}


	
} 
bool gen_export_json_mesh(ExportOptions* opts) {
	ScenePack *pack = (ScenePack *)opts->dataClass;
	json_t *mesh_data = json_object();
	for (int i = 0; i < pack->num_meshes; i++) {
		add_mesh_to_json(mesh_data, pack->m_meshes[i]);
	}
	char *s = json_dumps(mesh_data, JSON_INDENT(1));
	FILE *fd = fopen(opts->path, "w");
	fwrite(s, strlen(s), 1, fd);
	fclose(fd);
	return false;
}