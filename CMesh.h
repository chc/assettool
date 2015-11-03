#ifndef _CMESH_H
#define _CMESH_H
#include <stdint.h>
#define MAX_MESH_TEXTURES 4
class CMaterial;
class CMesh { 
public:
	CMesh();
	~CMesh();
	void setNumVerts(int count);
	void setVerticies(float *verts);
	void setNormals(float *normals);
	void setColours(float *colours);
	void setUVWs(float *uvws, int layer);
	void setIndices(uint32_t *indices, int num_indices);

	float *getVerticies() { return m_vertices; }
	float *getNormals() { return m_normals;}
	float *getColours() { return m_vert_cols; }
	uint32_t *getIndices() { return m_indices;}
	int getNumVertices() { return m_num_vertices; }
	int getNumIndicies() { return m_num_indices;}

	float *getUVWs(int layer);

	void setMaterial(CMaterial *material);
	CMaterial *getMaterial();

private:
	CMaterial *mp_material;
	float *m_uvws[MAX_MESH_TEXTURES];
	float *m_vertices;
	int m_num_vertices;
	float *m_normals;
	float *m_vert_cols;
	uint32_t *m_indices;
	int m_num_indices;
};
#endif //_CMESH_H