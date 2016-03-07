#include <Vector.h>
#include <Iterator.h>
#include "json.h"
#include <jansson.h>
#include <Generic/CKeyframeSequence.h>
#include <Generic/CKeyframeSeqCollection.h>
#include <Generic/CKeyframeCollection.h>
#include <Generic/ScenePack.h>
bool gen_import_json_mesh(ImportOptions* opts) {
	return false;
}
void add_mesh_to_json(json_t *jobj, CMesh *mesh) {


	json_t* jarr1 = json_array();
	int num_verts = mesh->getNumVertices();
	float *p = mesh->getVerticies();
	json_t *material_checksum = NULL;
	json_object_set_new(jobj, "name", json_string(mesh->getName()));
	printf("associated mat: %p\n", mesh->getMaterial());
	if (mesh->getMaterial()) {
		json_object_set_new(jobj, "material_checksum", json_integer(mesh->getMaterial()->getIdentifierChecksum()));
	}
	
	//add verts to json
	for (int i = 0; i < num_verts; i++) {
		json_t* v_array = json_array();
		for (int j = 0; j < 3; j++) {
			double v = (*p);
			json_array_append_new(v_array, json_real(v));
			p++;
		}
		p++; //skip W
		json_array_append(jarr1, v_array);
		json_decref(v_array);
	}
	json_object_set(jobj, "vertices", jarr1);
	json_decref(jarr1);
	
	jarr1 = json_array();
	//normals
	p = mesh->getNormals();
	if (p) {
		for (int i = 0; i < num_verts; i++) {
			json_t* v_array = json_array();
			for (int j = 0; j < 3; j++) {
				double v = (*p);
				json_array_append_new(v_array, json_real(v));
				p++;
			}
			p++; //skip W
			json_array_append(jarr1, v_array);
			json_decref(v_array);
		}
		json_object_set(jobj, "normals", jarr1);
		json_decref(jarr1);
	}
	
	//uvs
	json_t *uv_array = json_array();
	if (mesh->getUVWs(0) != NULL) {
		for (int l = 0; l < mesh->getUVLayers() || l == 0; l++) {
			p = mesh->getUVWs(l);
			for (int i = 0; i < num_verts; i++) {
				json_t* v_array = json_array();
				for (int j = 0; j < 3; j++) {
					double v = (*p);
					json_array_append_new(v_array, json_real(v));
					p++;
				}
				p++; //skip W
				json_array_append(uv_array, v_array);
				json_decref(v_array);
			}
		}
		json_object_set(jobj, "uvs", uv_array);
	}
	json_decref(uv_array);


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
					int index = *(indices++);
					json_array_append_new(v_array, json_integer(index));
				}
				json_array_append(indices_array, v_array);
				json_decref(v_array);
			}
			json_array_append(indices_array2, indices_array);
			json_decref(indices_array);
		}
		if (sub_indices) {
			json_object_set_new(jobj, "submesh_materials", json_integer(1));
		}
		json_object_set(jobj, "indices", indices_array2);
		json_decref(indices_array2);
	}	


	//dump bone weights
	int num_weight_sets = mesh->getNumWeightSets();
	json_t *weights_array = json_array();
	for(int i=0;i<num_weight_sets;i++) {
		json_t *weight_set = json_array();
		int num_weights;
		float *weights = mesh->getWeightsFloat(i, num_weights);
		for(int j=0;j<mesh->getNumVertices();j++) {
			json_t *jweights = json_array();
			for(int k=0;k<4;k++) {
				float v = *weights++;
				json_array_append_new(jweights, json_real(v));
			}

			json_array_append(weight_set, jweights);
			json_decref(jweights);
		}
		json_array_append(weights_array, weight_set);

		json_decref(weight_set);
	}

	json_object_set(jobj, "weights", weights_array);
	json_decref(weights_array);
	

	//dump bone indices
	int num_bone_index_sets = mesh->getNumBoneIndexSets();
	printf("%d set\n", num_bone_index_sets);
	json_t *bones_array = json_array();
	for(int i=0;i<num_bone_index_sets;i++) {
		json_t *indices_array = json_array();
		//uint32_t *getBoneIndicesUInt32(int set, int &num_indices);
		int num_indices;
		uint32_t *bone_indices = mesh->getBoneIndicesUInt32(0, num_indices);
		for(int j=0;j<mesh->getNumVertices();j++) {
			json_t *bone_group = json_array();
			for(int k=0;k<4;k++) {
				uint32_t v = *bone_indices++;
				json_array_append_new(bone_group, json_integer(v));
			}
			json_array_append(indices_array, bone_group);
			json_decref(bone_group);
		}
		json_array_append(bones_array, indices_array);
	}
	json_object_set(jobj, "bone_indices", bones_array);
	json_decref(bones_array);
}
void add_model_skeleton_to_json(json_t *jobj, CMesh *mesh) {
	int num_bones = mesh->getNumBones();

	json_t *bones = json_array();

	for(int i=0;i<num_bones;i++) {

		json_t* bone_obj = json_object();

		sBone *bone = mesh->getBone(i);

		float *matrix = (float *)&bone->matrix;
		float *mat = matrix;

		const char *name = (const char *)bone->identifier.sUnion.mString;
		//printf("Skeleton name: %s\n", name);
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
			json_array_append(bone_matrix, element_array);
			json_decref(element_array);
		}
		json_object_set(bone_obj, "matrix", bone_matrix);
		json_decref(bone_matrix);
		
		json_array_append(bones, bone_obj);
		json_decref(bone_obj);
		
	}
	json_object_set(jobj, "skeleton", bones);
	json_decref(bones);
}
void add_material_to_json(json_t *jobj, CMaterial *mat) {
	CTexture *tex;
	json_t *mat_obj = json_object();
	json_object_set_new(mat_obj, "checksum", json_integer(mat->getIdentifierChecksum()));
	json_object_set_new(mat_obj, "name", json_string(mat->getName()));
	bool wrote_materials = false;
	json_t *textures = json_array();
	for (int i = 0; (tex = mat->getTexture(i)) != NULL; i++) {
		json_t *tex_obj = json_object();
		const char *file_name = strrchr(tex->getPath(), '\\');
		json_object_set_new(tex_obj, "level", json_integer(i));
		if (file_name) {
			file_name++;
			json_object_set_new(tex_obj, "path", json_string(file_name));
		}
		json_array_append(textures, tex_obj);
		json_decref(tex_obj);
		wrote_materials = true;
	}
	if (!wrote_materials) {
		for (int i = 0; i < MAX_MATERIAL_TEXTURES; i++) {
			json_t *tex_obj = json_object();
			const char *name = mat->getTextureName(i);
			if(strlen(name) > 0) {
				json_object_set_new(tex_obj, "name", json_string(name));
			} else {
				uint32_t checksum = mat->getTextureChecksum(i);
				if (checksum == 0)  {
					//not adding this record
					json_decref(tex_obj);
					break;
				}			
				json_object_set_new(tex_obj, "checksum", json_integer(checksum));
			}
			
			json_array_append(textures, tex_obj);
			json_decref(tex_obj);
		}
		json_object_set(mat_obj, "textures", textures);
	}
	json_decref(textures); //we added the object, but its going out of scope, and only retained in mat_obj
	json_array_append(jobj, mat_obj);
	json_decref(mat_obj); //we added the object, but its going out of scope, and only retained in jobj
}
void json_export_scenepack(ExportOptions *opts) {
	ScenePack *pack = (ScenePack *)opts->dataClass;
	json_t *json_file = json_object();
	
	json_t *meshes = json_array();
	for (int i = 0; i < pack->num_meshes; i++) {
		int x = 0;
		json_t *mesh_data = json_object();
		add_mesh_to_json(mesh_data, pack->m_meshes[i]);
		add_model_skeleton_to_json(mesh_data, pack->m_meshes[i]);
		json_array_append(meshes, mesh_data);
		json_decref(mesh_data);
	}
	
	json_object_set(json_file, "meshes", meshes);
	json_decref(meshes);

	json_t *material_array = json_array();
	for (int i = 0; i < pack->num_materials; i++) {
		add_material_to_json(material_array, pack->m_materials[i]);
	}
	json_object_set(json_file, "materials", material_array);
	json_decref(material_array);

	json_dump_file(json_file, opts->path, JSON_INDENT(1)|JSON_PRESERVE_ORDER);

	json_decref(json_file);
}
void json_export_keyframe_seq(json_t *file, CKeyframeSequence *seq) {
	json_t *obj = json_object();

	json_t *vec_array = json_array();
	glm::vec3 position = seq->getPosition();

	json_array_append_new(vec_array, json_real(position.x));
	json_array_append_new(vec_array, json_real(position.y));
	json_array_append_new(vec_array, json_real(position.z));

	json_object_set(obj, "position", vec_array);
	json_decref(vec_array);

	vec_array = json_array();

	glm::quat quat = seq->getRotation();
	json_array_append_new(vec_array, json_real(quat.x));
	json_array_append_new(vec_array, json_real(quat.y));
	json_array_append_new(vec_array, json_real(quat.z));
	json_array_append_new(vec_array, json_real(quat.w));

	json_object_set(obj, "rotation", vec_array);

	json_object_set_new(obj, "time", json_real(seq->getBeginFrame()));
	json_decref(vec_array);

	json_array_append(file, obj);
	json_decref(obj);
}
void json_export_keyframe_collection(json_t *file, CKeyframeCollection *col) {
	static int idx = 0;
	printf("Export: %s : %d\n",col->getIdentifier().sUnion.mString, idx++);
	Core::Vector<CKeyframeSeqCollection *> key_col = col->getCollection();
	Core::Iterator<Core::Vector<CKeyframeSeqCollection *>, CKeyframeSeqCollection *> it = key_col.begin(); 

	json_object_set_new(file, "name", json_string(col->getIdentifier().sUnion.mString));
	json_t *keyframe_col = json_array();
	while(it != key_col.end()) {
		json_t *keyframe_seq_col_obj = json_object();
		CKeyframeSeqCollection *seqcol = *it;
		json_object_set_new(keyframe_seq_col_obj, "name", json_string(seqcol->getIdentifier().sUnion.mString));
		Core::Vector<CKeyframeSequence *> seq_vec =  seqcol->getCollection();
		Core::Iterator<Core::Vector<CKeyframeSequence *>, CKeyframeSequence *> it2 = seq_vec.begin();
		json_t *keyframe_seq_array = json_array();
		while(it2 != seq_vec.end()) {
			CKeyframeSequence *seq = *it2;
			json_export_keyframe_seq(keyframe_seq_array, seq);
			it2++;
		}
		json_object_set(keyframe_seq_col_obj, "keyframes", keyframe_seq_array);
		json_decref(keyframe_seq_array);

		json_array_append(keyframe_col, keyframe_seq_col_obj);
		it++;
	}

	json_object_set(file, "objects", keyframe_col);
	json_decref(keyframe_col);
	
}
void json_export_keyframe_collections(ExportOptions *opts) {
	json_t *keyframe_collections = json_array();
	Core::Vector<CKeyframeCollection *> *collections = (Core::Vector<CKeyframeCollection *> *)opts->dataClass;

	Core::Iterator<Core::Vector<CKeyframeCollection *> , CKeyframeCollection *> it = collections->begin();
	while(it != collections->end()) {
		json_t *keyframe_obj = json_object();
		CKeyframeCollection *key_col = *it;
		json_export_keyframe_collection(keyframe_obj, key_col);
		it++;
		json_array_append(keyframe_collections, keyframe_obj);
		json_decref(keyframe_obj);
	}
	json_dump_file(keyframe_collections, opts->path, JSON_INDENT(1)|JSON_PRESERVE_ORDER);
	json_decref(keyframe_collections);

}
bool gen_export_json_mesh(ExportOptions* opts) {
	switch(opts->type) {
		case ClassType_KeyframeCol:
			json_export_keyframe_collections(opts);
		break;
		case ClassType_ScenePack:
			json_export_scenepack(opts);
		break;
	}
	return false;
}