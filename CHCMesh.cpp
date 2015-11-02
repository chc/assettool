#include "CHCMesh.h"
#include "CMesh.h"
#include "crc32.h"
#include "ScenePack.h"
#define CHCMESH_VERSION 2
enum ECHCMeshFlags {
	ECHCMeshFlag_ColAsInt = (1<<0),
	ECHCMeshFlag_HasNormals = (1<<1),
	ECHCMeshFlag_HasCol = (1<<2),
	ECHCMeshFlag_HasUVs = (1<<3),
};
typedef struct {
	uint32_t version;
	uint32_t num_verts;
	uint32_t num_indices;
	uint8_t flags;
	uint32_t num_uvlayers;
} CHCMeshHead;
bool chc_engine_import_mesh(ImportOptions* opts) {
	return false;
}

void write_mesh(CMesh *mesh, FILE* fd) {

	uint32_t num_verts = mesh->getNumVertices();
	fwrite(&num_verts,sizeof(uint32_t),1,fd);

	float *verts = mesh->getVerticies();
	float *normals = mesh->getNormals();
	float *colours = mesh->getColours();
	float *uvs = mesh->getUVWs(0);

	uint32_t flags = 0;

	uint32_t material_checksum = 0;

	CMaterial *mat = mesh->getMaterial();
	if(mat != NULL) {
		material_checksum = crc32(0,mat->getName(),strlen(mat->getName()));
	}

	uint32_t stride = 0;
	if(normals) {
		flags |= ECHCMeshFlag_HasNormals;
		stride += sizeof(float) * 3;
	}
	if(colours) {
		flags |= ECHCMeshFlag_HasCol;
		stride += sizeof(float);
	}
	if(uvs) {
		flags |= ECHCMeshFlag_HasUVs;
		stride += sizeof(float) * 2;
	}
	fwrite(&stride,sizeof(uint32_t),1,fd);
	fwrite(&material_checksum,sizeof(uint32_t),1,fd);
	for(uint32_t i=0;i<num_verts;i++) {
		fwrite(verts,sizeof(float),3,fd);
		verts += 3;
		if(colours != NULL) {
			fwrite(colours,sizeof(float),1,fd);
			colours++;
		}
		if(normals != NULL) {
			fwrite(normals,sizeof(float),3,fd);
			normals +=  3;
		}
		if(uvs != NULL) {
			fwrite(uvs,sizeof(float),2,fd);
			uvs += 3; //W component is skipped
		}
	}
}

void write_material(CMaterial *material, FILE* fd) {
	uint32_t checksum = crc32(0,material->getName(),strlen(material->getName()));
	fwrite(&checksum,sizeof(uint32_t),1,fd);

	float col[4], s;

	material->getSpecColour(col[0],col[1],col[2],col[3]);
	fwrite(&col,sizeof(col),1,fd);
	material->getAmbientColour(col[0],col[1],col[2],col[3]);
	fwrite(&col,sizeof(col),1,fd);
	material->getDiffuseColour(col[0],col[1],col[2],col[3]);
	fwrite(&col,sizeof(col),1,fd);

	s = material->getShine();
	fwrite(&s,sizeof(float),1,fd);

	s = material->getShineStrength();
	fwrite(&s,sizeof(float),1,fd);

	//write textures
	CTexture *tex;
	bool tile[2];
	char c = 0;
	int i = 0;

	uint32_t tex_count = 0;
	while(material->getTexture(i++) != NULL) {
		tex_count++;
	}
	i=0;
	fwrite(&tex_count,sizeof(uint32_t),1,fd);
	do {
		tex = material->getTexture(i++);
		if(tex == NULL) break;
		checksum = crc32(0,tex->getPath(),strlen(tex->getPath()));
		fwrite(&checksum,sizeof(uint32_t),1,fd);
		tex->getOffset(col[0], col[1]);
		fwrite(&col,sizeof(float),2,fd);
		tex->getTile(tile[0], tile[1]);
		c = tile[0] != 0;
		fwrite(&c,sizeof(char),1,fd);
		c = tile[1] != 0;
		fwrite(&c,sizeof(char),1,fd);		
	} while(tex != NULL);
}
bool chc_engine_export_mesh(ExportOptions* opts) {
	FILE *fd = fopen(opts->path, "wb");
	ScenePack *scenepack = (ScenePack *)opts->dataClass;


	fwrite(&scenepack->num_meshes,sizeof(uint32_t),1,fd);
	for(int i=0;i<scenepack->num_meshes;i++) {
		write_mesh(scenepack->m_meshes[i],fd);
	}
	fwrite(&scenepack->num_materials,sizeof(uint32_t),1,fd);
	for(int i=0;i<scenepack->num_materials;i++) {
		write_material(scenepack->m_materials[i],fd);
	}
	
	fclose(fd);
	return false;
}