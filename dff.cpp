#include "main.h"
#include "CGame.h"
#include "dff.h"

#include <stdlib.h>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CMaterial.h"
#include "CMesh.h"
#include "ScenePack.h"

typedef struct {
	uint32_t tag;
	uint32_t size;
	uint32_t version;
} DFFChunkInfo;
typedef struct {
	uint32_t num_atomics;
	uint32_t unknown[2];
} DFFClumpHeader;

enum EDFFVertexFlags {
	EDFFVertFlag_TriStrip = (1<<0),
	EDFFVertFlag_HasPosition = (1<<1),
	EDFFVertFlag_HasUV = (1<<2),
	EDFFVertFlag_HasColour = (1<<3),
	EDFFVertFlag_HasNormal = (1<<4),
	EDFFVertFlag_Light = (1<<5), //light the vert..?
	EDFFVertFlag_ModuleMaterials = (1<<6),
	EDFFVertFlag_Textured = (1<<7),
};
typedef struct {
	glm::mat3 rotation_matrix;
	glm::vec3 position;
	uint32_t parent_frame;
	uint32_t flags;
} FrameInfo;

typedef struct {
	uint16_t filter_flags;
	uint16_t unknown;
	char texturename[64];
	char alphaname[64];
}TextureRecord;

typedef struct {
	uint32_t integer_1;
	uint32_t colour;
	uint32_t integer_2;
	uint32_t texture_count;
	std::vector<TextureRecord *> textures;
	glm::vec3 unknown;
} MaterialRecord;

typedef struct {
	uint16_t flags;
	uint16_t unknown_count;
	uint32_t face_count;
	uint32_t vertex_count;
	uint32_t frame_count;

	glm::vec3 **uv_data;
	glm::vec3 *vertex_data;
	glm::vec3 *normal_data;

	uint32_t *vertex_colours;

	uint8_t uvcount;
	uint8_t unknown;

	glm::vec4 bounding_sphere; //xyzr

	glm::ivec4 *indicies;

	uint32_t unknown_postbs[2];

	std::vector<MaterialRecord *> m_material_records;

	std::map<int, std::vector<glm::ivec3>> m_index_buffers;

} GeometryRecord;

typedef struct {
	std::vector<GeometryRecord *> m_geom_records;
	

	MaterialRecord *last_material;
	GeometryRecord *last_geometry;
} DFFInfo;
typedef struct {
	uint32_t material_count;
	uint32_t unknown[4];
} MaterialHeader;
void dump_vec3(glm::vec3 vec) {
	printf("%f %f %f\n",vec.x,vec.y,vec.z);
}
void dump_mat3(glm::mat3 mat) {
	printf("%f %f %f\n",mat[0][0],mat[0][1],mat[0][2]);
	printf("%f %f %f\n",mat[1][0],mat[1][1],mat[1][2]);
	printf("%f %f %f\n",mat[2][0],mat[2][1],mat[2][2]);
}
void dump_frame_info(FrameInfo *frame) {
	printf("Rotation: ");
	dump_mat3(frame->rotation_matrix);
	printf("Position: ");
	dump_vec3(frame->position);

	printf("Parent: %d\n", frame->parent_frame);
	printf("Flags: %d\n",frame->flags);
}
void dump_clump_header(DFFClumpHeader *head) {
	printf("Num Atomics: %d\n",head->num_atomics);
}
void dump_header(DFFChunkInfo *head) {
	printf("tag: %08X -  %d\n", head->tag, head->tag);
	printf("size: %d\n", head->size);
	printf("version: %08X\n", head->version);
}
enum DFFTags {
	DFFTag_rwDATA = 1,
	DFFTag_rwSTRING = 2,
	DFFTag_rwEXTENSION = 3,
	DFFTag_rwTEXTURE = 6,
	DFFTag_rwMATERIALLIST = 8,
	DFFTag_rwMATERIAL = 7,
	DFFTag_rwFRAMELIST = 14,
	DFFTag_rwGEOMETRY = 15,
	DFFTag_rwCLUMP = 16,
	DFFTag_rwATOMIC = 20,
	DFFTag_rwGEOMETRYLIST = 26,
	DFFTag_rwANIMPLUGIN = 286,
	DFFTag_rwMATERLIALEFFECTS = 288,
	DFFTag_rwMATERIALSPLIT = 1294,
	DFFTag_rwFRAME = 0x253F2FE,
	DFFTag_nvCOLOURS = 0x253F2F9,
};
bool parse_chunk(DFFInfo *dff_out, DFFChunkInfo *chunk, FILE *fd, DFFTags last_tag = (DFFTags)-1) {
	dump_header(chunk);
	DFFChunkInfo clumpHead;
	switch(chunk->tag) {
		case DFFTag_rwCLUMP: {
				fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd);
				parse_chunk(dff_out,&clumpHead, fd,(DFFTags)chunk->tag);
				break;
		}
		case DFFTag_rwFRAMELIST: {
				fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd);
				parse_chunk(dff_out,&clumpHead, fd,(DFFTags)chunk->tag);
			break;
		}
		case DFFTag_rwGEOMETRYLIST: {
				fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd);
				parse_chunk(dff_out,&clumpHead, fd,(DFFTags)chunk->tag);
			break;
		}
		case DFFTag_rwATOMIC: {
			static int icount = 0;
			printf("Skipping atomic %d at %08X...\n",icount++,ftell(fd));
			fseek(fd, chunk->size, SEEK_CUR); //skip atomic data
			break;
	  }
		case DFFTag_rwMATERIALLIST: {
			fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd);
			parse_chunk(dff_out,&clumpHead, fd,(DFFTags)chunk->tag);
			break;
		}
		case DFFTag_rwTEXTURE: {
			fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd);
			parse_chunk(dff_out,&clumpHead, fd,(DFFTags)chunk->tag);
			break;
		}
		case DFFTag_rwEXTENSION: {
			fseek(fd, chunk->size, SEEK_CUR);
			if(last_tag == (DFFTags)-1) {
				return false;
			} else if(last_tag == DFFTag_rwMATERIAL) {
				
				/*
				glm::vec4 unknown_vec;
				uint32_t unknown_ints[2];
				fread(glm::value_ptr(unknown_vec), sizeof(float),4, fd);
				fread(&unknown_ints, sizeof(uint32_t), 2, fd);
				*/
			}
			break;
		 }
		case DFFTag_rwMATERIAL: {
			MaterialRecord *mat = new MaterialRecord;
			memset(mat, 0, sizeof(mat));
			fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd); //read data header

			fread(&mat->integer_1, sizeof(uint32_t), 1, fd);
			fread(&mat->colour, sizeof(uint32_t), 1, fd);
			fread(&mat->integer_2, sizeof(uint32_t), 1, fd);
			fread(&mat->texture_count, sizeof(uint32_t), 1, fd);
			fread(glm::value_ptr(mat->unknown), sizeof(float), 3, fd);
			dff_out->last_material = mat;

			for(int i=0;i<mat->texture_count;i++) {
				fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd);
				parse_chunk(dff_out,&clumpHead, fd,(DFFTags)chunk->tag);
			}
			dff_out->last_geometry->m_material_records.push_back(mat);
			printf("Read mat extenstion at %08X\n",ftell(fd));
			//read material extension
			fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd);
			//fseek(fd, clumpHead.size, SEEK_CUR);
			
			parse_chunk(dff_out,&clumpHead, fd,(DFFTags)chunk->tag);
			break;
		}
		case DFFTag_rwGEOMETRY: {
			fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd);
			GeometryRecord *rec = new GeometryRecord;
			//memset(rec,0,sizeof(GeometryRecord));
			rec->vertex_count = 0;
			rec->face_count = 0;
			rec->normal_data = NULL;
			rec->vertex_data = NULL;
			rec->uv_data = NULL;
			rec->indicies = NULL;
			fread(&rec->flags, sizeof(uint16_t), 1, fd);
			fread(&rec->uvcount, sizeof(uint8_t), 1, fd);
			fread(&rec->unknown, sizeof(uint8_t), 1, fd);
			fread(&rec->face_count, sizeof(uint32_t), 1, fd);
			fread(&rec->vertex_count, sizeof(uint32_t), 1, fd);
			fread(&rec->frame_count, sizeof(uint32_t), 1, fd);
			rec->m_index_buffers = std::map<int, std::vector<glm::ivec3>>();
			rec->uv_data = (glm::vec3**)malloc(sizeof(glm::vec3*) * rec->uvcount);
			for(int i=0;i<rec->uvcount;i++) {
				rec->uv_data[i] = new glm::vec3[rec->vertex_count];
			}

			if(rec->flags & EDFFVertFlag_HasColour) {
				rec->vertex_colours = (uint32_t*)malloc(rec->vertex_count*sizeof(uint32_t));
				for(int i=0;i<rec->vertex_count;i++) {
					fread(&rec->vertex_colours[i], sizeof(uint32_t), 1, fd);
				}
			}
			for(int i=0;i<rec->uvcount;i++) {
				for(int j=0;j<rec->vertex_count;j++) {
					fread(glm::value_ptr(rec->uv_data[i][j]), sizeof(float),2,fd);
					rec->uv_data[i][j].z = 0.0;
					printf("%f %f\n",rec->uv_data[i][j].x,rec->uv_data[i][j].y);
				}
			}
			if(rec->face_count > 0) {
				rec->indicies = new glm::ivec4[rec->face_count];
				uint16_t last_material = -1;
				for(int i=0;i<rec->face_count;i++) {
					uint16_t indices[3], materialid;
					fread(&rec->indicies[i].x, sizeof(uint16_t),1,fd); //index x
					fread(&rec->indicies[i].y, sizeof(uint16_t),1,fd); //index y
					fread(&rec->indicies[i].w, sizeof(uint16_t), 1, fd); //materialid
					fread(&rec->indicies[i].z, sizeof(uint16_t),1,fd); //index z
					
					printf("%d %d %d %d\n",rec->indicies[i].x,rec->indicies[i].y,rec->indicies[i].z, rec->indicies[i].w);
				}
			}
			int pos = ftell(fd);

			fread(glm::value_ptr(rec->bounding_sphere), sizeof(float),4,fd);
			fread(&rec->unknown_postbs, sizeof(uint32_t), 2, fd);
			//fread(&rec->unknown, sizeof(uint32_t), 2, fd);
			printf("Vert Count: %d %08X\n",rec->vertex_count,pos);

			if(rec->flags & EDFFVertFlag_HasPosition) {
				rec->vertex_data = new glm::vec3[rec->vertex_count];
				for(int i=0;i<rec->vertex_count;i++) {
					fread(glm::value_ptr(rec->vertex_data[i]),sizeof(float),3,fd);
					printf("%f %f %f\n",rec->vertex_data[i].x,rec->vertex_data[i].y,rec->vertex_data[i].z);
				}
			}
			if(rec->flags & EDFFVertFlag_HasNormal) {
				rec->normal_data = new glm::vec3[rec->vertex_count];
				for(int i=0;i<rec->vertex_count;i++) {
					fread(glm::value_ptr(rec->normal_data[i]),sizeof(float),3,fd);
					printf("%f %f %f\n",rec->normal_data[i].x,rec->normal_data[i].y,rec->normal_data[i].z);
				}
			}
			dff_out->m_geom_records.push_back(rec);
			dff_out->last_geometry = rec;
			printf("Finish geom read at %08X\n",ftell(fd));
			break;
		}
		case DFFTag_rwDATA: {
			switch(last_tag) {
				case DFFTag_rwFRAMELIST: {
					uint32_t num_frames;
					fread(&num_frames, sizeof(uint32_t), 1, fd);
					printf("Num Frames: %d\n",num_frames);
					FrameInfo frame;
					for(int i=0;i<num_frames;i++) {
						fread(glm::value_ptr(frame.rotation_matrix), sizeof(float), 9, fd);
						fread(glm::value_ptr(frame.position), sizeof(float), 3, fd);
						fread(&frame.parent_frame, sizeof(uint32_t), 1, fd);
						fread(&frame.flags, sizeof(uint32_t), 1, fd);
						dump_frame_info(&frame);
					}
					char str[256];
					//read frame names
					for(int i=0;i<num_frames;i++) {
						memset(&str,0,sizeof(str));
						fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd); //data header
						
						fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd); //read string
						fread(&str, clumpHead.size, 1, fd);
						printf("%s\n",str);

					}

					break;
				 }
				case DFFTag_rwGEOMETRYLIST: {
					uint32_t num_geom;
					fread(&num_geom, sizeof(uint32_t),1, fd);
					printf("Geom Count: %d\n",num_geom);
					for(int i=0;i<num_geom;i++) {
						//read geometry info
						fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd);
						parse_chunk(dff_out,&clumpHead, fd,(DFFTags)chunk->tag);

						//read material list
						fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd);
						parse_chunk(dff_out,&clumpHead, fd,(DFFTags)chunk->tag);

						//read geom extensions
						fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd);
						parse_chunk(dff_out,&clumpHead, fd,(DFFTags)chunk->tag);
					}
					break;
				}
				case DFFTag_rwMATERIALLIST: {
					MaterialHeader mat;
					fread(&mat.material_count, sizeof(uint32_t), 1, fd);
					//fread(&mat.unknown, sizeof(uint32_t), 4, fd);
					fseek(fd, sizeof(uint32_t)*mat.material_count,SEEK_CUR);
					printf("Reading %d matmerials\n",mat.material_count);

					for(int i=0;i<mat.material_count;i++) {
						fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd);
						parse_chunk(dff_out,&clumpHead, fd,(DFFTags)chunk->tag);
					}
					break;
				}
				case DFFTag_rwTEXTURE: {
					uint16_t flags, unknown;
					TextureRecord *texrec = new TextureRecord;
					fread(&texrec->filter_flags, sizeof(uint16_t), 1, fd);
					fread(&texrec->unknown, sizeof(uint16_t), 1, fd);
					char str[256];

					fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd); //read string
					fread(&texrec->texturename, clumpHead.size, 1, fd);

					fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd); //read string 2... only found it empty so far
					fread(&texrec->alphaname, clumpHead.size, 1, fd);

					printf("%s\n",texrec->texturename);
					printf("%s\n",texrec->alphaname);

					
					dff_out->last_material->textures.push_back(texrec);

					fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd); //read empty extension..
					break;
				}
				case DFFTag_rwCLUMP: {
						DFFClumpHeader chead;
						fread(&chead, sizeof(chead), 1, fd);
						dump_clump_header(&chead);
					break;
				 }
			}
			break;
		}
	}
	return true;
}

CTexture *load_texture(const char *path, bool tile_u, bool tile_v, float u_offset, float v_offset); //in cchcmaxxml.cpp
void getMaterialFromRecord(MaterialRecord *matrec, CMaterial *mat) {
	float r = ((matrec->colour>>24)&0xff) / 255.0,g = ((matrec->colour>>16)&0xff) / 255.0,b= ((matrec->colour>>8)&0xff) / 255.0,a = (matrec->colour&0xff) / 255.0;
	mat->getDiffuseColour(r,g,b,a);

	char name[64];
	sprintf(name,"%p",mat);
	mat->setName(name);

	std::vector<TextureRecord *>::iterator it = matrec->textures.begin();
	int level = 0;
	while(it != matrec->textures.end()) {
		TextureRecord *texrec = *it;
		sprintf(name,"tex/%s.PNG",texrec->texturename);
		CTexture *tex = load_texture(name, true, true, 0.0, 0.0);
		mat->setTexture(tex, level++);
		it++;
	}
}
bool gta_rw_import_dff(ImportOptions* impOpts) {
	FILE *fd = fopen(impOpts->path, "rb");
	DFFChunkInfo head;
	DFFInfo info;
	memset(&info,0,sizeof(info));
	info.m_geom_records = std::vector<GeometryRecord *>();
	while(!feof(fd)) {
		fread(&head, sizeof(head), 1, fd);
		if(!parse_chunk(&info,&head, fd)) 
			break;
	}
	fclose(fd);


	int num_geom_records = info.m_geom_records.size();
	CMesh** meshes = (CMesh**)malloc(num_geom_records * sizeof(CMesh*));
	memset(meshes,0,num_geom_records * sizeof(CMesh*));


	std::vector<GeometryRecord *>::iterator it = info.m_geom_records.begin();
	int num_materials = 0, num_verts = 0;

	std::map<int, std::vector<glm::ivec3>> m_index_buffers;
	while(it != info.m_geom_records.end()) {
		GeometryRecord *rec = *it;

		num_materials += rec->m_material_records.size();
		num_verts += rec->vertex_count;
		int last_switchpoint = 0;
		uint16_t last_materialid = -1;
		for(int i=0;i<rec->face_count;i++) {
			glm::ivec4 indices = rec->indicies[i];
			rec->m_index_buffers[indices.w].push_back(glm::ivec3(indices.x, indices.y, indices.z));
		}
		it++;
	}

	CMesh **output_meshes = (CMesh**)malloc(info.m_geom_records.size() * sizeof(CMesh *));
	float *temp_verts = (float *)malloc(num_verts * sizeof(float) * 3);
	float *temp_verts_p = temp_verts;
	int mesh_id = 0;
	int num_meshes = 0;
	it = info.m_geom_records.begin();
	uint16_t **index_buffers = (uint16_t**)malloc(num_materials * sizeof(uint16_t*));
	memset(index_buffers,0,num_materials * sizeof(uint16_t*));
	int mesh_vert_count = 0;
	
	int index_buffer_idx = 0, mesh_buffer_idx = 0;

	std::vector<CMaterial *> materials;

	std::map<int, int> m_mat_instance_counts;
	while(it != info.m_geom_records.end()) {
		output_meshes[mesh_buffer_idx] = new CMesh();
		output_meshes[mesh_buffer_idx]->setUseIndexedMaterials(true);
		GeometryRecord *g = *it;
		std::map<int, int> found_materials; 
		for(int i=0;i<g->face_count;i++) {
			uint32_t matid = g->indicies[i].w;
			m_mat_instance_counts[matid]++;
		}

		struct MatIndicesInfo {
			uint32_t *head;
			uint32_t *p;
			int num_indices;
		};



		output_meshes[mesh_buffer_idx]->setIndexLevels(g->m_index_buffers.size());
		std::map<int, std::vector<glm::ivec3>>::iterator it2 = g->m_index_buffers.begin();
		int level = 0;
		while(it2 != g->m_index_buffers.end()) {
			std::pair<int, std::vector<glm::ivec3>> item = *it2;

			MaterialRecord *matrec = g->m_material_records[item.first];

			CMaterial *cmat = new CMaterial();
			getMaterialFromRecord(matrec, cmat);
			materials.push_back(cmat);
			output_meshes[mesh_buffer_idx]->setIndexMaterial(cmat, level);


			uint32_t *indices = (uint32_t*) malloc(sizeof(uint32_t) * 3 * item.second.size());
			uint32_t *p = indices;

			std::vector<glm::ivec3>::iterator it3 = item.second.begin();

			while(it3 != item.second.end()) {
				glm::ivec3 cur_indices = *it3;
				*p++ = cur_indices.x;
				*p++ = cur_indices.y;
				*p++ = cur_indices.z;
				it3++;
			}
			output_meshes[mesh_buffer_idx]->setIndices(indices, item.second.size(), level);
			free(indices);

			level++;
			it2++;
		}

		output_meshes[mesh_buffer_idx]->setNumVerts(g->vertex_count);
		for(int i=0;i<g->vertex_count;i++) {
			memcpy(temp_verts_p,glm::value_ptr(g->vertex_data[i]), sizeof(float) * 3);
			temp_verts_p += 3;
		}
		output_meshes[mesh_buffer_idx]->setVerticies(temp_verts);
		temp_verts_p = temp_verts;

		if(g->normal_data) {
			for(int i=0;i<g->vertex_count;i++) {
				memcpy(temp_verts_p,glm::value_ptr(g->normal_data[i]), sizeof(float) * 3);
				temp_verts_p += 3;
			}
			output_meshes[mesh_buffer_idx]->setNormals(temp_verts);
		}
		temp_verts_p = temp_verts;

		if(g->uv_data) {

			for(int i=0;i<g->uvcount;i++) {
				for(int j=0;j<g->vertex_count;j++) {
					memcpy(temp_verts_p,glm::value_ptr(g->uv_data[i][j]), sizeof(float) * 3);
					temp_verts_p += 3;
				}
				output_meshes[mesh_buffer_idx]->setUVWs(temp_verts, i);
			}
		}
		temp_verts_p = temp_verts;
		mesh_buffer_idx++;
		it++;
	}
	

	//run exporter
	ScenePack scene;
	memset(&scene,0,sizeof(scene));
	scene.m_meshes = (CMesh**)output_meshes;
	CMaterial **output_mats = (CMaterial**)malloc(materials.size() * sizeof(CMaterial*));
	std::vector<CMaterial *>::iterator matit = materials.begin();
	int i =0;
	while(matit != materials.end()) {
		output_mats[i++] = *matit;
		matit++;
	}
	scene.m_materials = (CMaterial**)output_mats;
	scene.num_materials = materials.size();
	scene.num_meshes = mesh_buffer_idx;

	scene.m_collision = NULL;
	
	ExportOptions opts;
	memset(&opts,0,sizeof(opts));
	
	opts.dataClass = &scene;
	opts.srcPath = impOpts->path;
	opts.args = impOpts->expArgs;
	opts.path = impOpts->outpath;
	impOpts->exporter(&opts);
	
	return true;
}
bool gta_rw_export_dff(ExportOptions *expOpts) {
	return false;
}