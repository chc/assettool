#ifndef _CMESH_H
#define _CMESH_H
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <Generic/CGeneric.h>
#include <Generic/CCollision.h>
#include <Generic/DataBank.h>
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
struct sFace {
	glm::ivec4 vertex_indices;
	int material_id;
};
struct sBone {
	float matrix[16];
	sBone *parent;
	sGenericData identifier;
};
enum EMeshDataBanks {
	EMeshDataBank_Vertices,
	EMeshDataBank_Normals,
	EMeshDataBank_Colours,
	EMeshDataBank_UVs,
	EMeshDataBank_Weights,
	EMeshDataBank_BoneIndices,
	EMeshDataBank_Indices,
	EMeshDataBank_Count,
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

	float *getVerticies();
	float *getNormals();
	uint32_t *getColours();
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

	/*
		Coordinate system related functions
	*/
		
	void setCoordinateSystem(ECoordinateSystem system) { m_coordinate_system = system; }
	ECoordinateSystem getCoordinateSystem(ECoordinateSystem handedness) { return m_coordinate_system; }
	void convertToCoordinateSystem(ECoordinateSystem system);

	/*
		Vertex face related functions
	*/
	void setNumFaces(uint32_t num_faces);
	sFace *getFace(int index);


	/*
		Vertex weight stuff
	*/

	void setWeightFlags(uint32_t type);
	uint32_t getWeightFlags();
	void setNumWeightSets(int num_sets);
	void setWeightsUInt32(int set, uint32_t *weights, int num_weights);
	void setWeightsFloat(int set, float *weights, int num_weights);
	float *getWeightsFloat(int set, int &num_weights);
	uint32_t *getWeightsUInt32(int set, int &num_weights);

	uint32_t getNumWeightSets();

	/*
		!!! All this bone crap must be fixed up into a struct (BoneInfo)
	*/

	/*
		All bone indices are 4 uint32_ts atm
	*/
	void setNumBoneIndexSets(int num_sets);
	void setBoneIndicesUInt32(int set, uint32_t *indices, int num_indices);
	void setBoneIndicesFloat(int set, float *values, int num_indices);

	uint32_t getNumBoneIndexSets();
	uint32_t *getBoneIndicesUInt32(int set, int &num_indices);
	float *getBoneIndicesFloat(int set, int &num_indices);

	/*
		Skeleton stuff
	*/
	void setNumBones(uint32_t num_bones);
	uint32_t getNumBones();
	sBone *getBone(uint32_t index); //used for accessing too
	sBone *getBoneByName(const char *name); //used by dff IO only.. please avoid.. this shouldn't exist

private:
	void generate_bbox();
	char m_name[64];
	CMaterial *mp_material;
	CCollision *mp_collision;
	int m_num_vertices;

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


	//mesh weighting stuff
	uint32_t m_weight_flags;
	int m_num_weightsets;
	//uint32_t *m_num_weights;
	//float **m_weights;

	int m_num_bone_index_sets;
	//uint32_t *m_num_bone_indices;
	//uint32_t **m_bone_indices;

	//old bone info
/*
	uint32_t m_num_inverse_bone_matrices;
	float **m_inverse_bone_matrices; //flat array of multiple matrices


	DataMapEntry *mp_bone_name_map;
	uint32_t m_num_bone_name_entries;

	uint32_t m_num_bone_parent_ids;
	uint32_t *mp_bone_parent_ids;
*/

	int m_num_faces;
	sFace* mp_faces;

	//new bone struct
	int m_num_bones;
	sBone*	mp_bone_info;

	CDataPackage* mp_data_package;
};
#endif //_CMESH_H