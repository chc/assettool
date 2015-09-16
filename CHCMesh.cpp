#include "CHCMesh.h"
#include "CMesh.h"
#define CHCMESH_VERSION 1
enum ECHCMeshFlags {
	ECHCMeshFlag_ColAsInt = 1,
	ECHCMeshFlag_HasNormals = 2,
	ECHCMeshFlag_HasCol = 4
};
typedef struct {
	uint32_t version;
	uint32_t num_verts;
	uint32_t num_indices;
	uint8_t flags;
} CHCMeshHead;
bool chc_engine_import_mesh(ImportOptions* opts) {
	return false;
}
bool chc_engine_export_mesh(ExportOptions* opts) {
	FILE *fd = fopen(opts->path, "wb");
	CMesh *mesh = (CMesh *)opts->dataClass;
	float *verts = mesh->getVerticies();
	float *normals = mesh->getNormals();
	uint32_t *indicies = mesh->getIndices();
	CHCMeshHead head;
	memset(&head,0,sizeof(head));
	head.version = CHCMESH_VERSION;
	head.num_indices = mesh->getNumIndicies();
	head.num_verts = mesh->getNumVertices();

	if(normals) head.flags |= ECHCMeshFlag_HasNormals;

	fwrite(&head,sizeof(head),1,fd);
	
	fwrite(verts,sizeof(float)*3,head.num_verts,fd);
	if(head.flags & ECHCMeshFlag_HasNormals) {
		fwrite(normals,sizeof(float)*3,head.num_verts,fd);
	}

	if(indicies) {
		fwrite(indicies,sizeof(uint32_t),head.num_indices,fd);
	}

	fclose(fd);
	return false;
}