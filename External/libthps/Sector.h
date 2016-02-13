#ifndef _LIBTHPS_SECTOR_H
#define _LIBTHPS_SECTOR_H
#include "LibTHPS.h"
#include "Mesh.h"
#include <vector>
namespace LibTHPS {

class Sector {
public:
	Sector(FILE *fd, EPlatform platform);
	~Sector();

	float *getPositions();
	float *getNormals();

	uint32_t *getWeights();
	uint16_t *getBoneIndices();

	uint32_t *getColours();
	uint32_t getNumVerticies();
	uint32_t getChecksum();

	uint32_t getNumTexChannels();
	float *getTexCoords();

	std::vector<Mesh *> getMeshes();
	static std::vector<Sector *> loadSectorsFromScene(FILE *fd, EPlatform platform, int num_sectors);
private:

	EPlatform m_platform;

	uint32_t m_checksum;
	uint32_t m_bone_idx;
	uint32_t m_flags;
	uint32_t m_num_mesh;

	float bbox[6];
	float bsphere[4];

	uint32_t billboard_type;
	float billboard_origin[3];
	float billboard_pivot_pos[3];
	float billboard_pivot_axis[3];

	uint32_t m_num_verts;
	uint32_t m_vertex_stride;
	float *mp_vertex_positions;

	float *mp_vertex_normals;
	uint32_t *mp_vertex_weights;
	uint16_t *mp_vertex_bone_indices;

	uint32_t m_num_tc_sets;
	float *mp_vertex_tex_coords;

	uint32_t *mp_vertex_colours;

	char *mp_vc_wibble_indices;

	std::vector<Material *> m_material_list;
	std::vector<Mesh *> m_meshes;

	void loadFromFile(FILE *fd);
};

}
#endif //_LIBTHPS_SECTOR_H