#include <Generic/CMesh.h>
#include <Generic/CMaterial.h>
#include <Generic/CCollision.h>
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
	mp_parent = NULL;
	num_index_levels = 0;
	m_num_uv_layers = 0;
	m_group_id = 0;
	m_num_materials = 0;
	has_sub_indices = false;
	m_indexed_materials = false;
	m_prim_type = CMeshPrimType_TriangleList;
	memset(&m_uvws, 0, sizeof(m_uvws));
	memset(&m_bbox, 0, sizeof(m_bbox));

	m_weight_flags = 0;
	m_num_weightsets = 0;
	m_num_weights = 0;
	m_weights = NULL;


	memset(&default_hierarchical_position,0, sizeof(default_hierarchical_position));
	memset(&default_hierarchical_rotation,0, sizeof(default_hierarchical_rotation));
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
		has_sub_indices = true;
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
	memcpy(default_hierarchical_rotation, pos, sizeof(float)*9);
}

float *CMesh::getDefaultHierarchialPosition() {
	return (float *)&default_hierarchical_position;
}
float *CMesh::getDefaultHiearchialRotation() {
	return (float *)&default_hierarchical_rotation;
}
void CMesh::setParent(CMesh *mesh) {
	mp_parent = mesh;
}
CMesh *CMesh::getParent() {
	return mp_parent;
}

bool CMesh::hasSubIndices() {
	return has_sub_indices;
}
COLBBox CMesh::getBBox() {
	if (m_bbox.checksum == 0)
		generate_bbox();
	return m_bbox;
}

void CMesh::generate_bbox() {
	COLBBox bbox;
	bbox.checksum = 1;
	float min[3];
	float max[3];
	
	for (int i = 0; i < 3; i++) {
		min[i] = 999999999.0;
		max[i] = -999999999.0;
	}

	float *verts = getVerticies();
	float *p = verts;
	for (int i = 0; i < m_num_vertices; i++) {
		for (int j = 0; j < 3; j++) {
			if (p[j] < min[j]) {
				min[j] = p[j];
			}
			if (p[j] > max[j]) {
				max[j] = p[j];
			}
		}
		p += 3;
	}

	for (int i = 0; i < 3; i++) {
		bbox.min[i] = min[i];
		bbox.max[i] = max[i];
	}
	m_bbox = bbox;
}

void CMesh::convertToHandedness(ECoordinateHandedness handedness) {
	if(m_handedness == handedness) return;
}
void CMesh::setNumWeightSets(uint32_t num_sets) {
	m_num_weightsets = num_sets;
	m_weights = (float **)malloc(sizeof(float *) * num_sets);
	m_num_weights = (uint32_t *)malloc(num_sets * sizeof(uint32_t));
}
void CMesh::setWeights(float *weights, uint32_t set, uint32_t num_weights) {
	m_weights[set] = (float *)malloc(sizeof(float) * 4 * num_weights);
	m_num_weights[set] = num_weights;
	memcpy(m_weights[set], weights, num_weights * sizeof(float) * 4);
}
float *CMesh::getWeights(uint32_t set, uint32_t &num_weights) {
	num_weights = m_num_weights[set];
	return m_weights[set];
}
uint32_t CMesh::getNumWeightSets() {
	return m_num_weightsets;
}

void CMesh::setWeightFlags(uint32_t type) {
	m_weight_flags = type;
}
uint32_t CMesh::getWeightFlags() {
	return m_weight_flags;
}

void CMesh::setNumInverseBoneMatrices(uint32_t num_matrices) {
	m_num_inverse_bone_matrices = num_matrices;
	m_inverse_bone_matrices = (float **)malloc(num_matrices * sizeof(float *));
}
void CMesh::setInverseBoneMatrices(float* matrices, uint32_t index) {
	m_inverse_bone_matrices[index] = (float *)malloc(sizeof(float) * (4*4));
	memcpy(m_inverse_bone_matrices[index], matrices, sizeof(float) * (4*4));
}
float* CMesh::getInverseBoneMatrices(uint32_t set) {
	return m_inverse_bone_matrices[set];
}

void CMesh::setNumBoneIndexSets(uint32_t num_sets) {
	m_num_bone_index_sets = num_sets;
	m_num_bone_indices = (uint32_t*) malloc(sizeof(uint32_t) * num_sets);
	m_bone_indices = (uint32_t **)malloc(sizeof(uint32_t *) * num_sets);
}
void CMesh::setBoneIndices(uint32_t set, uint32_t *indices, uint32_t num_indices) {
	m_num_bone_indices[set] = num_indices;
	m_bone_indices[set] = (uint32_t*)malloc(sizeof(uint32_t) * num_indices * 4);
	memcpy(m_bone_indices[set], indices, sizeof(uint32_t) * num_indices * 4);
}
uint32_t CMesh::getNumBoneIndexSets() {
	return m_num_bone_index_sets;
}
uint32_t *CMesh::getBoneIndices(uint32_t set, uint32_t &num_indices) {
	num_indices = m_num_bone_indices[set];
	return m_bone_indices[set];
}