#include "CHCMesh.h"
#include "CMesh.h"
#include "crc32.h"
#include "ScenePack.h"
#include "CImage.h"
#include <algorithm>

#define CHCMESH_VERSION 3
enum ECHCMeshFlags { //must corrospond to game
	ECHCMeshFlag_ColAsInt = (1<<0),
	ECHCMeshFlag_HasNormals = (1<<1),
	ECHCMeshFlag_HasCol = (1<<2),
	ECHCMeshFlag_HasUVs = (1<<3),
};
enum ECHCPrimType { //must corrospond to game
	ECHCPrimType_TriangleList,
	ECHCPrimType_TriangleStrips,
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
	uint8_t primtype = ECHCPrimType_TriangleList;

	if(mesh->getPrimType() == CMeshPrimType_TriangleStrips) {
		primtype = ECHCPrimType_TriangleStrips;
	}
	fwrite(&primtype, sizeof(uint8_t), 1, fd);

	float *verts = mesh->getVerticies();
	float *normals = mesh->getNormals();
	uint32_t *colours = mesh->getColours();
	float *float_colours = NULL;
	if(colours != NULL) {
		float_colours = (float *)malloc(num_verts * sizeof(float) * 4);
		float *p = float_colours;
		uint32_t *c = colours;
		for(int i=0;i<num_verts;i++) {
			uint32_t colour = *c++;
			float r = ((colour>>24)&0xff) / 255.0,g = ((colour>>16)&0xff) / 255.0,b= ((colour>>8)&0xff) / 255.0,a = (colour&0xff) / 255.0;
			*p++ = r;
			*p++ = g;
			*p++ = b;
			*p++ = a;
		}
	}
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
	if(float_colours) {
		flags |= ECHCMeshFlag_HasCol;
		stride += sizeof(float) * 4;
	}
	if(uvs) {
		flags |= ECHCMeshFlag_HasUVs;
		stride += sizeof(float) * 3;
	}
	fwrite(&flags,sizeof(uint32_t),1,fd);
	fwrite(&stride,sizeof(uint32_t),1,fd);
	fwrite(&material_checksum,sizeof(uint32_t),1,fd);

	uint32_t group_checksum = mesh->getGroupId();
	fwrite(&group_checksum, sizeof(uint32_t), 1, fd);
	for(uint32_t i=0;i<num_verts;i++) {
		fwrite(verts,sizeof(float),3,fd);
		verts += 3;
		if(float_colours != NULL) {
			fwrite(float_colours,sizeof(float),4,fd);
			float_colours += 4;
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
	int num_index_sets = mesh->getNumIndexLevels();
	
	if(num_index_sets == 0) {
		num_index_sets = 1;
		fwrite(&num_index_sets, sizeof(uint32_t), 1, fd);
		fwrite(&num_indicies,sizeof(uint32_t),1,fd);
		fwrite(mesh->getIndices(),sizeof(uint32_t) * 3,num_indicies,fd);
	} else {
		fwrite(&num_index_sets, sizeof(uint32_t), 1, fd);
		for(int i=0;i<num_index_sets;i++) {
			uint32_t* indices =  mesh->getIndices(i);
			num_indicies = mesh->getNumIndicies(i);
			fwrite(&num_indicies,sizeof(uint32_t),1,fd);
			fwrite(indices, sizeof(uint32_t) * 3, num_indicies, fd);
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

void write_collision(FILE *fd, CCollision *collision) {
	uint32_t version = CHCMESH_VERSION;
	fwrite(&version,sizeof(uint32_t),1,fd);

	std::vector<BBox> boxes = collision->getBBoxes();
	uint32_t num_bboxes = boxes.size();
	fwrite(&num_bboxes, sizeof(uint32_t), 1, fd);
	std::vector<BBox>::iterator it = boxes.begin();
	while(it != boxes.end()) {
		BBox box = *it;
		fwrite(&box.checksum, sizeof(uint32_t), 1, fd);
		fwrite(&box.min, sizeof(float), 3, fd);
		fwrite(&box.max, sizeof(float), 3, fd);
		it++;
	}
}
bool chc_engine_export_mesh(ExportOptions* opts) {
	char fname[FILENAME_MAX+1];
	sprintf(fname,"%s.mesh",opts->path);
	FILE *fd = fopen(fname, "wb");
	ScenePack *scenepack = (ScenePack *)opts->dataClass;

	uint32_t version = CHCMESH_VERSION;
	fwrite(&version,sizeof(uint32_t),1,fd);
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


	if(scenepack->m_collision) {
		sprintf(fname,"%s.col",opts->path);
		FILE *colfd = fopen(fname, "wb");
		write_collision(colfd, scenepack->m_collision);
		fclose(colfd);
	}
	return false;
}