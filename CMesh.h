#ifndef _CMESH_H
#define _CMESH_H
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define MAX_MESH_TEXTURES 4
class CMaterial;
class CCollision;
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
private:
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
};
#endif //_CMESH_H