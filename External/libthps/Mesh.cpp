#include "Mesh.h"
namespace LibTHPS {

Mesh::Mesh(FILE *fd, EPlatform platform) {
	m_platform = platform;
	memset(mp_indices,0,sizeof(mp_indices));
	memset(&m_num_indices,0,sizeof(m_num_indices));

	loadMesh(fd);
	
}
Mesh::~Mesh() {
	for(int i=0;i<MAX_LOD_INDICES;i++) {
		if(mp_indices[i] != NULL) {
			free(mp_indices[i]);
		}
	}
}
uint32_t Mesh::getCombinedIndices(uint16_t **indices) {
	uint32_t len = 0;
	uint16_t *p;
	for(int i=0;i<m_num_lod_index_levels;i++) {
		len += m_num_indices[i];
	}
	*indices = (uint16_t*)malloc(len * sizeof(uint16_t));
	memset(*indices,0,len);

	p = *indices;
	

	for(int i=0;i<m_num_lod_index_levels;i++) {
		if(mp_indices[i] != NULL) {
			memcpy(p,mp_indices[i],m_num_indices[i]*sizeof(uint16_t));
			p += m_num_indices[i];	
		}
	}
	return len;
}
void Mesh::loadMesh(FILE *fd) {

	fread(&cen,sizeof(float),3,fd);
	fread(&rad,sizeof(float),1,fd);
	fread(&inf,sizeof(float),3,fd);
	fread(&sup,sizeof(float),3,fd);

	fread(&m_flags,sizeof(uint32_t),1,fd);

	fread(&m_material_checksum,sizeof(uint32_t),1,fd);
	fread(&m_num_lod_index_levels,sizeof(uint32_t),1,fd);
	
	for(int i=0;i<m_num_lod_index_levels;i++) {
		fread(&m_num_indices[i],sizeof(uint32_t),1,fd);
		mp_indices[i] = (uint16_t*)malloc(m_num_indices[i]*sizeof(uint16_t));
		fread(mp_indices[i],sizeof(uint16_t),m_num_indices[i],fd);
	}
}
uint32_t *Mesh::getNumIndices() {
	return m_num_indices;
}
uint32_t Mesh::getNumLODIndicies() {
	return m_num_lod_index_levels;
}
uint16_t **Mesh::getIndices() {
	return mp_indices;
}
std::vector<Mesh *> Mesh::loadMeshesFromSector(FILE *fd, EPlatform platform, uint32_t num_meshes) {
	std::vector<Mesh *> ret;
	for(int i=0;i<num_meshes;i++) {
		ret.push_back(new Mesh(fd, platform));
	}
	return ret;
}
uint32_t	Mesh::getMaterialChecksum() {
	return m_material_checksum;
}

}