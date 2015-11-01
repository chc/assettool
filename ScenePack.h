#ifndef _SCENEPACK_H
#define _SCENEPACK_H
#include "CMaterial.h"
#include "CMesh.h"
#include "CTexture.h"
typedef struct {
	CMesh **m_meshes;
	uint32_t num_meshes;

	CMaterial **m_materials;
	uint32_t num_materials;
} ScenePack;
#endif //_SCEHEPACK_H