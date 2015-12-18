#ifndef _SCENEPACK_H
#define _SCENEPACK_H
#include "CMaterial.h"
#include "CMesh.h"
#include "CTexture.h"
#include "CCollision.h"
typedef struct {
	CMesh **m_meshes;
	uint32_t num_meshes;

	CMaterial **m_materials;
	uint32_t num_materials;

	CCollision *m_collision;
} ScenePack;

COLBBox get_scenepack_bbox(ScenePack *pack);

#endif //_SCEHEPACK_H