#ifndef _LIBTHPS_MESH_H
#define _LIBTHPS_MESH_H
#define MAX_LOD_INDICES 8

#include "LibTHPS.h"
#include <vector>
namespace LibTHPS {

class Mesh {
public:
	Mesh(FILE *fd, EPlatform platform);
	~Mesh();
	uint16_t **getIndices();

	uint32_t getCombinedIndices(uint16_t **indices); //indices must be freed after
	uint32_t *getNumIndices();
	uint32_t getNumLODIndicies();

	uint32_t	getMaterialChecksum();
	static std::vector<Mesh *> loadMeshesFromSector(FILE *fd, EPlatform platform, uint32_t num_meshes);
private:

	EPlatform m_platform;

	void loadFromFile(FILE *fd);

	float rad;
	float cen[3];
	float inf[3];
	float sup[3];

	uint32_t m_flags;

	uint32_t m_material_checksum;
	uint32_t m_num_lod_index_levels;

	uint16_t *mp_indices[MAX_LOD_INDICES];
	uint32_t m_num_indices[MAX_LOD_INDICES];

	void loadMesh(FILE *fd);
};

}
#endif //_LIBTHPS_MESH_H