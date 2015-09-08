#include "Scene.h"
#include "Sector.h"
#include <vector>

#include <Windows.h>

namespace LibTHPS {

Sector::Sector(FILE *fd, EPlatform platform) {
	mp_vertex_positions = NULL;
	mp_vertex_normals = NULL;
	mp_vertex_colours = NULL;
	mp_vertex_tex_coords = NULL;
	mp_vc_wibble_indices = NULL;
	mp_vertex_bone_indices = NULL;
	mp_vertex_weights = NULL;
	m_platform = platform;
	m_num_tc_sets = 0;
	loadFromFile(fd);
}
Sector::~Sector() {
	if(mp_vertex_positions)
		free(mp_vertex_positions);

	if(mp_vertex_normals)
		free(mp_vertex_normals);

	if(mp_vertex_tex_coords)
		free(mp_vertex_tex_coords);

	if(mp_vc_wibble_indices)
		free(mp_vc_wibble_indices);

	if(mp_vertex_bone_indices) {
		free(mp_vertex_bone_indices);
	}
	if(mp_vertex_weights) {
		free(mp_vertex_weights);
	}
}
void Sector::loadFromFile(FILE *fd) {

	fread(&m_checksum, sizeof(uint32_t), 1, fd);
	fread(&m_bone_idx, sizeof(uint32_t), 1, fd);
	fread(&m_flags, sizeof(uint32_t), 1, fd);

	fread(&m_num_mesh, sizeof(uint32_t), 1, fd);
	fread(&bbox[0], sizeof(float), 6, fd);
	fread(&bsphere[0], sizeof(float), 4, fd);

	if( m_flags & 0x00800000UL )
	{
		fread(&billboard_type, sizeof(uint32_t), 1, fd);
		fread(&billboard_origin[0], sizeof(float), 3, fd);
		fread(&billboard_pivot_pos, sizeof(float), 3, fd);
		fread(&billboard_pivot_axis, sizeof(float), 3, fd);
	}
	fread(&m_num_verts, sizeof(uint32_t), 1, fd);
	fread(&m_vertex_stride, sizeof(uint32_t), 1, fd);
	
	mp_vertex_positions = (float *)malloc(sizeof(float)*3*m_num_verts);
	fread(mp_vertex_positions, sizeof(float)*3,m_num_verts,fd);

	if( m_flags & 0x04) {
		mp_vertex_normals = (float *)malloc(sizeof(float)*3*m_num_verts);
		fread(mp_vertex_normals, sizeof(float)*3,m_num_verts,fd);
	}
	if(m_flags & 0x10) {
		mp_vertex_weights = (uint32_t*)malloc(sizeof(uint32_t)*m_num_verts);
		fread(mp_vertex_weights,sizeof(uint32_t),m_num_verts,fd);
		mp_vertex_bone_indices = (uint16_t*)malloc(sizeof(uint16_t)*4*m_num_verts);
		fread(mp_vertex_bone_indices, sizeof(uint16_t)*4,m_num_verts, fd);
	}

	if( m_flags & 0x01 )
	{
		fread(&m_num_tc_sets, sizeof(uint32_t), 1, fd);
		if(m_num_tc_sets > 0) {
			mp_vertex_tex_coords = (float*) malloc(m_num_verts * 2 * m_num_tc_sets * sizeof(float));
			fread(mp_vertex_tex_coords, sizeof(float)*2*m_num_tc_sets, m_num_verts, fd);
		}
	}

	if(m_flags & 0x02) {
		mp_vertex_colours = (uint32_t*)malloc(m_num_verts*sizeof(uint32_t));
		fread(mp_vertex_colours, sizeof(uint32_t), m_num_verts, fd);
	}

	if(m_flags & 0x800) {
		mp_vc_wibble_indices = (char *)malloc(sizeof(uint8_t)*m_num_verts);
		fread(mp_vc_wibble_indices, sizeof(uint8_t), m_num_verts, fd);
	}

	m_meshes = Mesh::loadMeshesFromSector(fd, m_platform, m_num_mesh);
}
uint32_t Sector::getNumTexChannels() {
	return m_num_tc_sets;
}
float *Sector::getTexCoords() {
	return mp_vertex_tex_coords;
}
uint32_t Sector::getChecksum() {
	return m_checksum;
}
uint32_t Sector::getNumVerticies() {
	return m_num_verts;
}
uint32_t *Sector::getColours() {
	return mp_vertex_colours;
}
float *Sector::getPositions() {
	return mp_vertex_positions;
}
float *Sector::getNormals() {
	return mp_vertex_normals;
}
	
std::vector<Mesh *> Sector::getMeshes() {
	return m_meshes;
}
std::vector<Sector *> Sector::loadSectorsFromScene(FILE *fd, EPlatform platform, int num_sectors) {
	std::vector<Sector *> ret;
	for(int i=0;i<num_sectors;i++) {
		ret.push_back(new Sector(fd, platform));
	}
	return ret;
}

}