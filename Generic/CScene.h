#ifndef _CSCENEPACK_H
#define _CSCENEPACK_H
#include <stdint.h>
typedef struct {
	CMesh **mp_meshes;
	uint32_t num_meshes;
	CMaterial **mp_materials
	uint32_t num_materials;
	CTexture **mp_textures;
	uint32_t num_textures;
} ScenePack;

void FreeScenePack(ScenePack* pack);
#endif //_CMESH_H