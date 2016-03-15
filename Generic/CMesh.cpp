#include <Generic/CMesh.h>
#include <Generic/CMaterial.h>
#include <Generic/CCollision.h>
#include <Generic/coordinate_normalizer.h>
#include <stdlib.h>
#include <string.h>
CMesh::CMesh() {
	m_indices = NULL;
	m_num_vertices = 0;
	m_num_indices = 0;
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
	memset(&m_bbox, 0, sizeof(m_bbox));

	m_weight_flags = 0;
	m_num_weightsets = 0;

	memset(&default_hierarchical_position,0, sizeof(default_hierarchical_position));
	memset(&default_hierarchical_rotation,0, sizeof(default_hierarchical_rotation));

	m_num_bones = 0;
	m_num_bone_index_sets = 0;

	mp_bone_info = NULL;
	mp_data_package = new CDataPackage(EMeshDataBank_Count);
}
CMesh::~CMesh() {
	if(mp_bone_info)
		free(mp_bone_info);

	if(mp_material) {
		free(mp_material);
	}
	delete mp_data_package;
}
void CMesh::setNumVerts(int count) {
	m_num_vertices = count;
}
void CMesh::setUVWs(float *uvs, int layer) {
	if(layer > m_num_uv_layers)
		m_num_uv_layers = layer;
	mp_data_package->GetDataBank(EMeshDataBank_UVs)->SetDataVector(layer, uvs, m_num_vertices, 3);
}
float *CMesh::getUVWs(int layer) {
	return mp_data_package->GetDataBank(EMeshDataBank_UVs)->GetVertexHead(layer);
}
void CMesh::setVerticies(float *verts) {
	if(m_num_vertices == 0 || !verts) return;
	mp_data_package->GetDataBank(EMeshDataBank_Vertices)->SetDataVector(0, verts, m_num_vertices);
}
float *CMesh::getVerticies() {
	return mp_data_package->GetDataBank(EMeshDataBank_Vertices)->GetVertexHead(0);
}
void CMesh::setNormals(float *normals) {
	if(m_num_vertices == 0 || !normals) return;
	mp_data_package->GetDataBank(EMeshDataBank_Normals)->SetDataVector(0, normals, m_num_vertices);
}
float *CMesh::getNormals() {
	return mp_data_package->GetDataBank(EMeshDataBank_Normals)->GetVertexHead(0);
}
void CMesh::setColours(uint32_t *colours) { 
	if(m_num_vertices == 0 || !colours) return;
	mp_data_package->GetDataBank(EMeshDataBank_Colours)->SetDataUInt32(0, colours, m_num_vertices);
}
void CMesh::setIndexLevels(int levels) {
	num_index_levels = levels;
	mp_data_package->SetNumBanks(EMeshDataBank_Indices, levels);


	//allocate materials
	mp_material = (CMaterial *)malloc(num_index_levels * sizeof(CMaterial *));
	m_num_materials = num_index_levels;
}
int CMesh::getNumIndicies(int layer) { 
	if(layer == -1) layer = 0;
	int ds = mp_data_package->GetNumElements(EMeshDataBank_Indices, layer);
	return ds;
}
uint32_t *CMesh::getIndices(int level) {
	uint32_t *ret = mp_data_package->GetDataBank(EMeshDataBank_Indices)->GetUInt32Head(level);
	printf("Indices: %d %p\n",level,ret);
	return ret;
}
void CMesh::setIndices(uint32_t *indices, int num_indices, int level) {
	mp_data_package->GetDataBank(EMeshDataBank_Indices)->SetDataUInt32(level, indices, num_indices);
}
void CMesh::setMaterial(CMaterial *material) {
	mp_material = material;
}
CMaterial *CMesh::getMaterial() {
	if(m_num_materials > 1 || num_index_levels != 0) {
		return ((CMaterial**)mp_material)[0];
	}
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

void CMesh::setNumWeightSets(int num_sets) {
	m_num_weightsets = num_sets;
	mp_data_package->GetDataBank(EMeshDataBank_Weights)->SetNumDataSets(num_sets);
}
void CMesh::setWeightsFloat(int set, float *weights, int num_weights) {
	mp_data_package->GetDataBank(EMeshDataBank_Weights)->SetDataVector(set, weights, num_weights, 4);
}
void CMesh::setWeightsUInt32(int set, uint32_t *weights, int num_weights) {
	mp_data_package->GetDataBank(EMeshDataBank_Weights)->SetDataUInt32(set, weights, num_weights);
}
float *CMesh::getWeightsFloat(int set, int &num_weights) {
	num_weights = mp_data_package->GetDataBank(EMeshDataBank_Weights)->GetNumDataSets(set);
	return mp_data_package->GetDataBank(EMeshDataBank_Weights)->GetVertexHead(set);
}
uint32_t *CMesh::getWeightsUInt32(int set, int &num_weights) {
	num_weights = mp_data_package->GetDataBank(EMeshDataBank_Weights)->GetNumDataSets(set);
	return mp_data_package->GetDataBank(EMeshDataBank_Weights)->GetUInt32Head(set);
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

void CMesh::setNumBoneIndexSets(int num_sets) {
	m_num_bone_index_sets = num_sets;
	mp_data_package->GetDataBank(EMeshDataBank_BoneIndices)->SetNumDataSets(num_sets);
}
void CMesh::setBoneIndicesUInt32(int set, uint32_t *indices, int num_indices) {
	mp_data_package->GetDataBank(EMeshDataBank_BoneIndices)->SetDataUInt32(set, indices, num_indices);	
}
void CMesh::setBoneIndicesFloat(int set, float *values, int num_indices) {
	mp_data_package->GetDataBank(EMeshDataBank_BoneIndices)->SetDataFloat(set, values, num_indices);
}
uint32_t CMesh::getNumBoneIndexSets() {
	return m_num_bone_index_sets;
}
uint32_t *CMesh::getBoneIndicesUInt32(int set, int &num_indices) {
	num_indices = mp_data_package->GetDataBank(EMeshDataBank_BoneIndices)->GetNumDataSets(set);
	return mp_data_package->GetDataBank(EMeshDataBank_BoneIndices)->GetUInt32Head(set);
}


void CMesh::convertToCoordinateSystem(ECoordinateSystem system) {
	//if(m_vertices)
		//convert_xyz_from_to(m_coordinate_system, system, m_vertices, m_num_vertices);
	mp_data_package->GetDataBank(EMeshDataBank_Vertices)->ConvertToCoordinateSystem(system);
	mp_data_package->GetDataBank(EMeshDataBank_Normals)->ConvertToCoordinateSystem(system);

	//for(int i=0;i<m_num_uv_layers;i++) {
		//convert_uvw_from_to(m_coordinate_system, system, m_uvws[i], m_num_vertices);
	//}

	m_coordinate_system = system;
}

void CMesh::setNumBones(uint32_t num_bones) {
	m_num_bones = num_bones;
	mp_bone_info = (sBone *)malloc(num_bones * sizeof(sBone));
	memset(mp_bone_info, 0, num_bones * sizeof(sBone));
}
sBone *CMesh::getBone(uint32_t index) {
	return &mp_bone_info[index];
}
sBone *CMesh::getBoneByName(const char *name) {
	for(int i=0;i<m_num_bones;i++) {
		if(mp_bone_info[i].identifier.type == EDataType_String_ASCII) {
			if(!strcmp(mp_bone_info[i].identifier.sUnion.mString, name)) {
				return &mp_bone_info[i];
			}
		}
	}
	return NULL;
}
uint32_t CMesh::getNumBones() {
	return m_num_bones;
}
uint32_t *CMesh::getColours() 
{
	return mp_data_package->GetDataBank(EMeshDataBank_Colours)->GetUInt32Head(0);
}