#include "CMesh.h"
#include <stdlib.h>
#include <string.h>
CMesh::CMesh() {
	m_indices = NULL;
	m_num_vertices = 0;
	m_num_indices = 0;
	m_normals = NULL;
	m_vert_cols = NULL;
	m_vertices = NULL;
}
CMesh::~CMesh() {
	if(m_vertices)
		free(m_vertices);
	if(m_normals)
		free(m_normals);
	if(m_vert_cols)
		free(m_vert_cols);
	if(m_indices)
		free(m_indices);
}
void CMesh::setNumVerts(int count) {
	m_num_vertices = count;
}
void CMesh::setVerticies(float *verts) {
	if(m_vertices || m_num_vertices == 0) return;
	m_vertices = (float *)malloc(m_num_vertices * sizeof(float) * 3);
	memcpy(m_vertices,verts,sizeof(float) * m_num_vertices * 3);
}
void CMesh::setNormals(float *normals) {
	if(m_normals || m_num_vertices == 0) return;
	m_normals = (float *)malloc(m_num_vertices * sizeof(float) * 3);
	memcpy(m_normals,normals,sizeof(float) * m_num_vertices * 3);
}
void CMesh::setColours(float *colours) { 
	if(m_vert_cols || m_num_vertices == 0) return;
	m_vert_cols = (float *)malloc(m_num_vertices * sizeof(float) * 3);
	memcpy(m_vert_cols,colours,sizeof(float) * m_num_vertices * 3);
}
void CMesh::setIndices(uint32_t *indices, int num_indices) {
	if(m_indices || num_indices == 0) return;
	m_indices = (uint32_t *)malloc(num_indices * sizeof(uint32_t) * 3);
	memcpy(m_indices,indices,num_indices * sizeof(uint32_t) * 3);
}