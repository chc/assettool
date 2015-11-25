#include "CMesh.h"
#include "CMaterial.h"
#include "CCollision.h"
#include <stdlib.h>
#include <string.h>
CMesh::CMesh() {
	m_indices = NULL;
	m_num_vertices = 0;
	m_num_indices = 0;
	m_normals = NULL;
	m_vert_cols = NULL;
	m_vertices = NULL;
	mp_material = NULL;
	mp_collision = NULL;
	num_index_levels = 0;
	m_group_id = 0;
	m_num_materials = 0;
	m_indexed_materials = false;
	m_prim_type = CMeshPrimType_TriangleList;
	memset(&m_uvws, 0, sizeof(m_uvws));

	memset(&default_hierarchical_position,0, sizeof(default_hierarchical_position));
	memset(&default_hierarchical_rotation,0, sizeof(default_hierarchical_rotation));
	default_hierarchical_rotation[0] = 1.0;
	default_hierarchical_rotation[3] = 1.0;
	default_hierarchical_rotation[6] = 1.0;
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
void CMesh::setUVWs(float *uvs, int layer) {
	m_uvws[layer] =  (float *)malloc(m_num_vertices * sizeof(float) * 3);
	memcpy(m_uvws[layer],uvs,m_num_vertices * sizeof(float) * 3);
	if(layer > m_num_uv_layers) {
		m_num_uv_layers = layer;
	}
}
float *CMesh::getUVWs(int layer) {
	return m_uvws[layer];
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
void CMesh::setColours(uint32_t *colours) { 
	if(m_vert_cols || m_num_vertices == 0) return;
	m_vert_cols = (uint32_t *)malloc(m_num_vertices * sizeof(uint32_t));
	memcpy(m_vert_cols,colours,m_num_vertices * sizeof(uint32_t));
}
void CMesh::setIndexLevels(int levels) {
	num_index_levels = levels;
	m_indices = (uint32_t *)malloc(num_index_levels * sizeof(uint32_t* ) * 3);
	memset(m_indices,0,num_index_levels * sizeof(uint32_t* ) * 3);
	m_num_indexed_levels = (int*)malloc(sizeof(int) * levels);

	//allocate materials
	mp_material = (CMaterial *)malloc(num_index_levels * sizeof(CMaterial *));

	m_num_materials = num_index_levels;
}
int CMesh::getNumIndicies(int layer) { 
	if(layer == -1) {
		return m_num_indices;
	} else {
		return m_num_indexed_levels[layer];
	}
}
uint32_t *CMesh::getIndices(int level) {
	if(level == -1) {
		return m_indices;
	} else {
		return ((uint32_t**)m_indices)[level];
	}
}
void CMesh::setIndices(uint32_t *indices, int num_indices, int level) {
	if(level < 0) {
		m_indices = (uint32_t *)malloc(num_indices * sizeof(uint32_t) * 3);
		memcpy(m_indices,indices,num_indices * sizeof(uint32_t) * 3);
	} else {
		m_num_indexed_levels[level] = num_indices;
		((uint32_t**)m_indices)[level] = (uint32_t*) malloc(num_indices * sizeof(uint32_t) * 3);
		memcpy(((uint32_t**)m_indices)[level],indices,num_indices * sizeof(uint32_t) * 3);
	}
	m_num_indices = num_indices;
}
void CMesh::setMaterial(CMaterial *material) {
	mp_material = material;
}
CMaterial *CMesh::getMaterial() {
	return mp_material;
}

void CMesh::setIndexMaterial(CMaterial *material, int level) {
	((CMaterial**)mp_material)[level] = material;
}
CMaterial *CMesh::getIndexMaterial(int level) {
	return ((CMaterial**)mp_material)[level];
}

void CMesh::setGroupId(uint32_t id) {
	m_group_id = id;
}
uint32_t CMesh::getGroupId() {
	return m_group_id;
}

void CMesh::setCollision(CCollision *collision) {
	mp_collision = collision;
}
CCollision *CMesh::getCollision() {
	return mp_collision;
}
void CMesh::setDefaultHierarchicalPosition(float *pos) {
	memcpy(default_hierarchical_position, pos, sizeof(float)*3);

}
void CMesh::setDefaultHierarchicalRotation(float *pos) {
	memcpy(default_hierarchical_rotation, pos, sizeof(float)*16);
}

float *CMesh::getDefaultHierarchialPosition() {
	return (float *)&default_hierarchical_position;
}
float *CMesh::getDefaultHiearchialRotation() {
	return (float *)&default_hierarchical_rotation;
}