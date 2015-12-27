#ifndef _CMESH_H
#define _CMESH_H
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <Generic/CGeneric.h>
#include <Generic/CCollision.h>
#define MAX_MESH_TEXTURES 4
class CMaterial;
class CKeyframeSequence;
class CCollision;
enum CMeshWeightTypeFlags {
	CMeshWeightTypeFlags_HasInverseBoneMatrices = (1<<0),
	CMeshWeightTypeFlags_HasBoneIndices = (1<<1),
};
enum CMeshPrimType {
	CMeshPrimType_Quads,
	CMeshPrimType_TriangleList,
	CMeshPrimType_TriangleStrips,
	CMeshPrimType_TriangleFans
};
class CMesh { 
public:
	CMesh();
	~CMesh();
	void setNumVerts(int count);
	void setVerticies(float *verts);
	void setNormals(float *normals);
	void setColours(uint32_t *colours);
	void setUVWs(float *uvws, int layer);

	void setIndexLevels(int levels);
	void setIndices(uint32_t *indices, int num_indices, int level = -1);

	float *getVerticies() { return m_vertices; }
	float *getNormals() { return m_normals;}
	uint32_t *getColours() { return m_vert_cols; }
	uint32_t *getIndices(int level = -1);
	int getNumVertices() { return m_num_vertices; }
	int getNumIndicies(int layer = -1);

	int getUVLayers() { return m_num_uv_layers; };
	float *getUVWs(int layer);

	void setGroupId(uint32_t id);
	uint32_t getGroupId();


	uint32_t getNumIndexLevels() { return num_index_levels; }
	//don't call these if you use index materials
	void setMaterial(CMaterial *material);
	CMaterial *getMaterial();
	bool hasMaterialArray();

	void setIndexMaterial(CMaterial *material, int level);
	CMaterial *getIndexMaterial(int level);
	int getNumMaterials() { /*if(mp_material != NULL && m_num_materials == 0) { return -1; }*/ return m_num_materials; }

	void setCollision(CCollision *collision);
	CCollision *getCollision();

	void setPrimType(CMeshPrimType type) { m_prim_type = type; }
	CMeshPrimType getPrimType() { return m_prim_type; }

	void setUseIndexedMaterials(bool set) { m_indexed_materials = set; };
	bool getUseIndexedMaterials() { return m_indexed_materials; }

	void setName(const char *name) { strcpy(m_name, name); }
	const char *getName() { return m_name; }

	void setDefaultHierarchicalPosition(float *pos);
	void setDefaultHierarchicalRotation(float *pos);

	float *getDefaultHierarchialPosition();
	float *getDefaultHiearchialRotation();

	void setNumKeyframeSequences(int num_sequences);//call before setting any as the array will be NULL
	int getNumKeyframeSequences();

	void setKeyframeSequence(int sequence_index, CKeyframeSequence *sequence);
	CKeyframeSequence getKeyframeSequence(int sequence_index);

	void setParent(CMesh *mesh);
	CMesh *getParent();

	bool hasSubIndices();

	COLBBox getBBox();
		
	void setCoordinateSystem(ECoordinateSystem system) { m_coordinate_system = system; }
	ECoordinateSystem getCoordinateSystem(ECoordinateSystem handedness) { return m_coordinate_system; }
	void convertToCoordinateSystem(ECoordinateSystem system);

	void setWeightFlags(uint32_t type);
	uint32_t getWeightFlags();
	void setNumWeightSets(uint32_t num_sets);
	void setWeights(float *weights, uint32_t set, uint32_t num_weights);
	float *getWeights(uint32_t set, uint32_t &num_weights);
	uint32_t getNumWeightSets();

	/*
		All inverse bone matrices are mat4 atm
	*/
	void setNumInverseBoneMatrices(uint32_t num_matrices);
	void setInverseBoneMatrices(float* matrices, uint32_t index);
	float* getInverseBoneMatrices(uint32_t set);

	/*
		All bone indices are 4 uint32_ts atm
	*/
	void setNumBoneIndexSets(uint32_t num_sets);
	void setBoneIndices(uint32_t set, uint32_t *indices, uint32_t num_indices);

	uint32_t getNumBoneIndexSets();
	uint32_t *getBoneIndices(uint32_t set, uint32_t &num_indices);
private:
	void generate_bbox();
	char m_name[64];
	CMaterial *mp_material;
	CCollision *mp_collision;
	float *m_uvws[MAX_MESH_TEXTURES];
	float *m_vertices;
	int m_num_vertices;
	float *m_normals;
	uint32_t *m_vert_cols;
	int num_index_levels;
	int *m_num_indexed_levels;
	uint32_t *m_indices;
	int m_num_indices;
	int m_num_uv_layers;
	uint32_t m_group_id;
	CMeshPrimType m_prim_type;
	bool m_indexed_materials;
	int m_num_materials;


	float default_hierarchical_position[3];
	float default_hierarchical_rotation[9];

	CMesh *mp_parent;

	bool has_sub_indices;

	COLBBox m_bbox;

	ECoordinateSystem m_coordinate_system;

	uint32_t m_weight_flags;
	uint32_t m_num_weightsets;
	uint32_t *m_num_weights;
	float **m_weights;

	uint32_t m_num_inverse_bone_matrices;
	float **m_inverse_bone_matrices; //flat array of multiple matrices

	uint32_t m_num_bone_index_sets;
	uint32_t *m_num_bone_indices;
	uint32_t **m_bone_indices;

	bool m_has_material_array;
};
#endif //_CMESH_H