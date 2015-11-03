#include "CHCMesh.h"
#include "CMesh.h"
#include "crc32.h"
#include "ScenePack.h"
#include "CImage.h"
#include <algorithm>

#define CHCMESH_VERSION 2
enum ECHCMeshFlags {
	ECHCMeshFlag_ColAsInt = (1<<0),
	ECHCMeshFlag_HasNormals = (1<<1),
	ECHCMeshFlag_HasCol = (1<<2),
	ECHCMeshFlag_HasUVs = (1<<3),
};
bool chc_engine_import_mesh(ImportOptions* opts) {
	return false;
}

void write_texture(CTexture *tex, FILE *fd) {
	tex->compress();

	uint32_t w,h;
	CImage *img = tex->getImage();
	img->getDimensions(w,h);

	uint32_t checksum = tex->getChecksum();
	fwrite(&checksum, sizeof(uint32_t), 1, fd);
	fwrite(&w,sizeof(uint32_t),1,fd);
	fwrite(&h,sizeof(uint32_t),1,fd);
	char colType = img->getColourType();
	fwrite(&colType,sizeof(char),1,fd);

	void *data = img->getRawData();

	uint32_t size = img->getDataSize();
	fwrite(&size, sizeof(uint32_t), 1, fd);
	fwrite(data,size,1,fd);


}

void write_mesh(CMesh *mesh, FILE* fd) {

	uint32_t num_verts = mesh->getNumVertices();
	uint32_t num_indicies = mesh->getNumIndicies();
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

	uint32_t stride = sizeof(float) * 3;
	if(normals) {
		flags |= ECHCMeshFlag_HasNormals;
		stride += sizeof(float) * 3;
	}
	if(colours) {
		flags |= ECHCMeshFlag_HasCol;
		stride += sizeof(float) * 3;
	}
	if(uvs) {
		flags |= ECHCMeshFlag_HasUVs;
		stride += sizeof(float) * 3;
	}
	fwrite(&flags,sizeof(uint32_t),1,fd);
	fwrite(&stride,sizeof(uint32_t),1,fd);
	fwrite(&material_checksum,sizeof(uint32_t),1,fd);
	for(uint32_t i=0;i<num_verts;i++) {
		fwrite(verts,sizeof(float),3,fd);
		verts += 3;
		if(colours != NULL) {
			fwrite(colours,sizeof(float),3,fd);
			colours += 3;
		}
		if(normals != NULL) {
			fwrite(normals,sizeof(float),3,fd);
			normals +=  3;
		}
		if(uvs != NULL) {
			fwrite(uvs,sizeof(float),3,fd);
			uvs += 3;
		}
	}

	fwrite(&num_indicies,sizeof(uint32_t),1,fd);
	fwrite(mesh->getIndices(),sizeof(uint32_t) * 3,num_indicies,fd);
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
	uint8_t c = 0;
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
		fwrite(&c,sizeof(uint8_t),1,fd);
		c = tile[1] != 0;
		fwrite(&c,sizeof(uint8_t),1,fd);		
	} while(tex != NULL);
}
bool chc_engine_export_mesh(ExportOptions* opts) {
	char fname[FILENAME_MAX+1];
	sprintf(fname,"%s.mesh",opts->path);
	FILE *fd = fopen(fname, "wb");
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

	sprintf(fname,"%s.tex",opts->path);
	FILE *texfd = fopen(fname, "wb");

	std::vector<CTexture *> textures;
	for(int i=0;i<scenepack->num_materials;i++) {
		//materials[i]->getTexture(0)
		for(int j=0;j<MAX_MATERIAL_TEXTURES;j++) {
			CTexture *tex = scenepack->m_materials[i]->getTexture(j);
			if(tex != NULL) {
				textures.push_back(tex);
			}
		}		
	}
	std::sort( textures.begin(), textures.end() );
	textures.erase( std::unique( textures.begin(), textures.end() ), textures.end() );

	uint32_t num_textures = textures.size();
	fwrite(&num_textures,sizeof(uint32_t),1,texfd);

	std::vector<CTexture *>::iterator it = textures.begin();
	while(it != textures.end()) {
		write_texture(*it, texfd);
		it++;
	}
	fclose(texfd);
	return false;
}