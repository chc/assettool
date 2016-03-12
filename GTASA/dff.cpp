#include <main.h>
#include <Generic/CGame.h>
#include "dff.h"

#include <stdlib.h>
#include <Vector.h>
#include <Map.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Generic/CMaterial.h>
#include <Generic/CMesh.h>
#include <Generic/ScenePack.h>

#include <crc32.h>
#include <algorithm>

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
enum EDFFTextureFilterModes {
	EDFFTexFilterMode_None,
	EDFFTexFilterMode_Nearest,
	EDFFTexFilterMode_Linear,
	EDFFTexFilterMode_MipNearest,
	EDFFTexFilterMode_MipLinear,
	EDFFTexFilterMode_LinearMipNearest,
	EDFFTexFilterMode_LinearMipLinear,
};
enum EDFFTextureAddresingModes {
	EDFFTextureAddressMode_NoTiling,
	EDFFTextureAddressMode_Wrap,
	EDFFTextureAddressMode_Mirror,
	EDFFTextureAddressMode_Clamp,
	EDFFTextureAddressMode_Border,
};
typedef struct 
{
	glm::mat3x3 rotation_matrix;
	glm::vec3 position;
	uint32_t parent_frame;
	uint32_t flags;
	char name[64];
	uint32_t bone_id; //bone id this belongs to, or -1
} FrameInfo;

typedef struct {
	uint32_t bone_number;
	uint32_t bone_id;
	uint32_t bone_type;
	uint32_t bone_file_index; //position, which it appears in the file
	float matrix[16];
} DFFBone;

typedef struct {
	uint16_t filter_flags;
	uint16_t unknown;
	char texturename[64];
	char alphaname[64];

	ETextureFilterMode mat_filter_mode;
	ETextureAddresingMode u_mode, v_mode;
}TextureRecord;

typedef struct {
	uint32_t flags;
	uint32_t colour;
	uint32_t integer_2;
	uint32_t texture_count;
	Core::Vector<TextureRecord *> textures;
	
	/**
 * \ingroup fundtypesdatatypes
 * \struct RwSurfaceProperties
 *  This type represents the ambient, diffuse and
 * specular reflection coefficients of a particular geometry. Each coefficient
 * is specified in the range 0.0 (no reflection) to 1.0 (maximum reflection). 
 * Note that currently the specular element is not used.
 */
	float ambient;   /**< ambient reflection coefficient */
    float specular;  /**< specular reflection coefficient */
    float diffuse;   /**< reflection coefficient */
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

	Core::Vector<MaterialRecord *> m_material_records;

	Core::Map<int, Core::Vector<glm::ivec3> > m_index_buffers;

	char name[64];

	Core::Vector<FrameInfo *> keyframes;

	glm::vec3 default_hierarchical_position;
	glm::mat3x3 default_hierarchical_rotation;
	FrameInfo *parent_frame;

	uint32_t output_mesh_id; //where is it in the outputmeshes buffer 

	//relies on vert count
	float *weights;
	uint8_t *bone_indices;

	//relies on bone count
	uint32_t bone_count;
	uint8_t *bone_used;
} GeometryRecord;

class DFFInfo {
public:
	std::vector<GeometryRecord *> m_geom_records;
	std::vector<FrameInfo *> m_frames;
	std::vector<DFFBone *> m_bones;

	MaterialRecord *last_material;
	GeometryRecord *last_geometry;

	uint32_t last_frame_index;

	DataMapEntry *frame_map;

	uint32_t last_bone_id; //or -1

};
typedef struct {
	uint32_t material_count;
	uint32_t unknown[4];
} MaterialHeader;

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
	DFFTag_rwLIGHT = 18,
	DFFTag_rwATOMIC = 20,
	DFFTag_rwGEOMETRYLIST = 26,
	DFFTag_rwANIMPLUGIN = 286,
	DFFTag_rwMATERLIALEFFECTS = 288,
	DFFTag_rwMATERIALSPLIT = 1294,
	DFFTag_rwFRAME = 0x253F2FE,
	DFFTag_nvCOLOURS = 0x253F2F9,
	DFFTag_HAnimPLG = 286,
	DFFTag_SkinPLG = 0x116,
	DFFTag_BreakablePLG = 0x253f2FD,
	DFFTag_BinMeshPLG = 0x50E,
};

DFFBone *get_dff_bone_by_index(DFFInfo *info, uint32_t index);

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
bool parse_chunk(DFFInfo *dff_out, DFFChunkInfo *chunk, FILE *fd, DFFTags last_tag = (DFFTags)-1) {
	//dump_header(chunk);
	DFFChunkInfo clumpHead;
	switch(chunk->tag) {
		case DFFTag_rwLIGHT: {
			uint32_t unknown_int;
			glm::vec4 unknown_vec;
			fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd); //read struct head
			fread(&unknown_int, sizeof(uint32_t), 1, fd);
			fread(glm::value_ptr(unknown_vec), sizeof(float), 4, fd);
			fread(&unknown_int, sizeof(uint32_t), 1, fd);

			fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd); //skip empty extension
			break;
		}
		case DFFTag_HAnimPLG:
		{
			uint32_t const_256, bone_id, num_bones, flags, data_size;
			fread(&const_256, sizeof(uint32_t), 1, fd);
			fread(&bone_id, sizeof(uint32_t), 1, fd);
			fread(&num_bones, sizeof(uint32_t), 1, fd);
			//skip unknown(possibly flag?) data
			if (num_bones > 0) {
				fread(&flags, sizeof(uint32_t), 1, fd);
				fread(&data_size, sizeof(uint32_t), 1, fd);
			}
			//save the bone id for frame reference
			dff_out->last_bone_id = bone_id;

			
			for (int i = 0; i < num_bones; i++) {
				DFFBone *bone = new DFFBone;
				fread(&bone->bone_id, sizeof(uint32_t), 1, fd);
				fread(&bone->bone_number, sizeof(uint32_t), 1, fd);
				fread(&bone->bone_type, sizeof(uint32_t), 1, fd);
				dff_out->m_bones.push_back(bone);
			}
			break;
		}
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
			uint32_t frame_index, geometry_index, unknown[2];
			printf("Skipping atomic %d at %08X...\n",icount++,ftell(fd));
			fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd);
			fread(&frame_index, sizeof(uint32_t), 1, fd);
			fread(&geometry_index, sizeof(uint32_t), 1, fd);
			fread(&unknown, sizeof(uint32_t), 2, fd);

			memset(&dff_out->m_geom_records[geometry_index]->name, 0, sizeof(dff_out->m_geom_records[geometry_index]->name));
			strcpy(dff_out->m_geom_records[geometry_index]->name, dff_out->m_frames[frame_index]->name);
			memcpy(&dff_out->m_geom_records[geometry_index]->default_hierarchical_position, glm::value_ptr(dff_out->m_frames[frame_index]->position), sizeof(float) * 3);
			memcpy(&dff_out->m_geom_records[geometry_index]->default_hierarchical_rotation, glm::value_ptr(dff_out->m_frames[frame_index]->rotation_matrix), sizeof(float) * 9);
			dff_out->m_geom_records[geometry_index]->keyframes.add(dff_out->m_frames[frame_index]);
			

			if (dff_out->m_frames[frame_index]->parent_frame != -1) {
				FrameInfo *parent = dff_out->m_frames[dff_out->m_frames[frame_index]->parent_frame];
				dff_out->m_geom_records[geometry_index]->parent_frame = parent;
			}
			else {
				dff_out->m_geom_records[geometry_index]->parent_frame = NULL;
			}

			printf("Atomic name: %d %s\n", frame_index, dff_out->m_frames[frame_index]->name);
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
			uint32_t read_size = 0;
			DFFChunkInfo subchunk;
			if(last_tag == DFFTag_rwGEOMETRYLIST) {
				while(read_size < chunk->size) {
					read_size += fread(&subchunk, sizeof(DFFChunkInfo), 1, fd);
					read_size += subchunk.size;
					parse_chunk(dff_out, &subchunk, fd, (DFFTags)last_tag);
				}
			} else if(chunk->size != 0) {
				fseek(fd, chunk->size, SEEK_CUR);
			}
			break;
		 }
		case DFFTag_BreakablePLG:
		case DFFTag_BinMeshPLG: {
			fseek(fd, chunk->size, SEEK_CUR);			
			break;
		}
		case DFFTag_SkinPLG: {
			uint8_t num_bones, num_used_bones, max_weights_per_vertex, padding;
			fread(&num_bones, sizeof(uint8_t), 1, fd);
			fread(&num_used_bones, sizeof(uint8_t), 1, fd);
			fread(&max_weights_per_vertex, sizeof(uint8_t), 1, fd);
			fread(&padding, sizeof(uint8_t), 1, fd);

			printf("aaa: %d %d\n", num_used_bones, max_weights_per_vertex);
			uint8_t *bones_used = (uint8_t *)malloc(num_used_bones * sizeof(uint8_t));
			fread(bones_used, num_used_bones, sizeof(uint8_t), fd);

			uint8_t *vertex_bone_indices = (uint8_t *)malloc(sizeof(uint8_t) * 4 * dff_out->last_geometry->vertex_count);
			float *weights = (float *)malloc(sizeof(float) * dff_out->last_geometry->vertex_count * 4);
			fread(vertex_bone_indices, dff_out->last_geometry->vertex_count * sizeof(uint8_t), 4, fd);

			fread(weights, dff_out->last_geometry->vertex_count * sizeof(float), 4, fd);

			//invert them for some reason??
			/*
			for(int i=0;i<dff_out->last_geometry->vertex_count;i++) {
				float *v = &weights[i * 4];
				float t[4];
				t[3] = v[0];
				t[2] = v[1];
				t[1] = v[2];
				t[0] = v[3];

				v[0] = t[0];
				v[1] = t[1];
				v[2] = t[2];
				v[3] = t[3];
			}
			*/
			
			//printf("Weights: \n");
			float *wp = weights;

			float wmin = 9999999.0;
			float wmax = -999999.0;
			for(int i=0;i<dff_out->last_geometry->vertex_count;i++) {
				printf("(%f,%f,%f,%f)\n",wp[0],wp[1],wp[2],wp[3]);
				for(int j=0;j<4;j++) {
					if(wp[j] < wmin) {
						wmin = wp[j];
					}
					if(wp[j] > wmax) {
						wmax = wp[j];
					}
				}
				wp += 4;
			}
			printf("Weights minmax: %f %f\n",wmin, wmax);
			for(int i=0;i<num_bones;i++) {
				DFFBone *bone = get_dff_bone_by_index(dff_out, i);
				fread(&bone->matrix, sizeof(float), 4*4, fd);
				bone->matrix[15] = 1.0;
				bone->bone_file_index = i;
			}

			dff_out->last_geometry->weights = weights;
			dff_out->last_geometry->bone_indices = vertex_bone_indices;
			dff_out->last_geometry->bone_count = num_bones;
			dff_out->last_geometry->bone_used = bones_used;
			
			
			break;
		 }
		case DFFTag_rwMATERIAL: {
			MaterialRecord *mat = new MaterialRecord;
			fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd); //read data header

			fread(&mat->flags, sizeof(uint32_t), 1, fd);
			fread(&mat->colour, sizeof(uint32_t), 1, fd);
			fread(&mat->integer_2, sizeof(uint32_t), 1, fd);
			fread(&mat->texture_count, sizeof(uint32_t), 1, fd);
			//fread(glm::value_ptr(mat->unknown), sizeof(float), 3, fd);
			fread(&mat->ambient, sizeof(float), 1, fd);
			fread(&mat->specular, sizeof(float), 1, fd);
			fread(&mat->diffuse, sizeof(float), 1, fd);
			dff_out->last_material = mat;

			for(int i=0;i<mat->texture_count;i++) {
				fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd);
				parse_chunk(dff_out,&clumpHead, fd,(DFFTags)chunk->tag);
			}
			dff_out->last_geometry->m_material_records.add(mat);
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
			rec->vertex_colours = NULL;
			rec->vertex_data = NULL;
			rec->uv_data = NULL;
			rec->indicies = NULL;
			fread(&rec->flags, sizeof(uint16_t), 1, fd);
			fread(&rec->uvcount, sizeof(uint8_t), 1, fd);
			fread(&rec->unknown, sizeof(uint8_t), 1, fd);
			fread(&rec->face_count, sizeof(uint32_t), 1, fd);
			fread(&rec->vertex_count, sizeof(uint32_t), 1, fd);
			fread(&rec->frame_count, sizeof(uint32_t), 1, fd);
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
					//printf("%f %f\n",rec->uv_data[i][j].x,rec->uv_data[i][j].y);
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
					
					//printf("%d %d %d %d\n",rec->indicies[i].x,rec->indicies[i].y,rec->indicies[i].z, rec->indicies[i].w);
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
					//printf("%f %f %f\n",rec->vertex_data[i].x,rec->vertex_data[i].y,rec->vertex_data[i].z);
				}
			}
			if(rec->flags & EDFFVertFlag_HasNormal) {
				rec->normal_data = new glm::vec3[rec->vertex_count];
				for(int i=0;i<rec->vertex_count;i++) {
					fread(glm::value_ptr(rec->normal_data[i]),sizeof(float),3,fd);
					//printf("%f %f %f\n",rec->normal_data[i].x,rec->normal_data[i].y,rec->normal_data[i].z);
				}
			}
			dff_out->m_geom_records.push_back(rec);
			dff_out->last_geometry = rec;
			printf("Finish geom read at %08X %d\n",ftell(fd), dff_out->m_geom_records.size());
			break;
		}
		case DFFTag_rwDATA: {
			switch(last_tag) {
				default: {
					fseek(fd, chunk->size, SEEK_CUR);
					break;
				}
				case DFFTag_rwFRAMELIST: {
					uint32_t num_frames;
					fread(&num_frames, sizeof(uint32_t), 1, fd);
					printf("Num Frames: %d\n",num_frames);
					FrameInfo *frame;
					for(int i=0;i<num_frames;i++) {
						frame = new FrameInfo;
						frame->flags = 0;
						frame->position = glm::vec3();
						frame->rotation_matrix = glm::mat3x3();
						frame->parent_frame = 0;
						frame->bone_id = -1;
						memset(&frame->name,0,sizeof(frame->name));
						fread(glm::value_ptr(frame->rotation_matrix), sizeof(float), 9, fd);
						fread(glm::value_ptr(frame->position), sizeof(float), 3, fd);
						fread(&frame->parent_frame, sizeof(uint32_t), 1, fd);
						fread(&frame->flags, sizeof(uint32_t), 1, fd);
						dff_out->m_frames.push_back(frame);
					}
					DataMapEntry *m_entries = (DataMapEntry *)malloc(sizeof(DataMapEntry) * num_frames);
					memset(m_entries, 0, sizeof(DataMapEntry) * num_frames);

					dff_out->frame_map = m_entries;
					//read frame names
					for(int i=0, x = 0;i<num_frames;i++) {
						dff_out->last_frame_index = i;
						fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd); //data header

						if (clumpHead.size > 0) {
							fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd); //read string
							if (clumpHead.tag == DFFTag_HAnimPLG) {
								parse_chunk(dff_out, &clumpHead, fd, (DFFTags)last_tag);
								fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd); //read string
							}
							if (clumpHead.tag == DFFTag_rwFRAME) {
								if(dff_out->last_bone_id != -1) {
									dff_out->m_frames[i]->bone_id = dff_out->last_bone_id;
									dff_out->last_bone_id = -1;
								}
								fread(&dff_out->m_frames[i]->name, clumpHead.size, 1, fd);
								//printf("read frame name: %s\n", dff_out->m_frames[i]->name);
								m_entries[x].value = dff_out->m_frames[i]->name;
								m_entries[x].identifier = x++;

							}
						}

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
						parse_chunk(dff_out,&clumpHead, fd,(DFFTags)last_tag);
					}
					break;
				}
				case DFFTag_rwMATERIALLIST: {
					MaterialHeader mat;
					fread(&mat.material_count, sizeof(uint32_t), 1, fd);
					//fread(&mat.unknown, sizeof(uint32_t), 4, fd);
					fseek(fd, sizeof(uint32_t)*mat.material_count,SEEK_CUR);
					printf("Reading %d materials\n",mat.material_count);

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
					EDFFTextureFilterModes filter_mode = (EDFFTextureFilterModes)(texrec->filter_flags & 0xFF);
					uint8_t addressing = (texrec->filter_flags & 0xFF << 8) >> 8;
					uint8_t addressing_v = (addressing & 0xF0) >> 4, addressing_u = addressing & 0x0F;

					ETextureFilterMode mat_filter_mode;
					switch (filter_mode) {
					default:
					case EDFFTexFilterMode_None:
						texrec->mat_filter_mode = ETexFilterMode_None;
						break;
					case EDFFTexFilterMode_Nearest:
						texrec->mat_filter_mode = ETexFilterMode_Nearest;
						break;
					case EDFFTexFilterMode_Linear:
						texrec->mat_filter_mode = ETexFilterMode_Linear;
						break;
					case EDFFTexFilterMode_MipNearest:
						texrec->mat_filter_mode = ETexFilterMode_MipNearest;
						break;
					case EDFFTexFilterMode_MipLinear:
						texrec->mat_filter_mode = ETexFilterMode_MipLinear;
						break;
					case EDFFTexFilterMode_LinearMipNearest:
						texrec->mat_filter_mode = ETexFilterMode_LinearMipNearest;
						break;
					case EDFFTexFilterMode_LinearMipLinear:
						texrec->mat_filter_mode = ETexFilterMode_LinearMipLinear;
						break;
					}
					ETextureAddresingMode u_mode, v_mode;
					switch (addressing_u) {
					case EDFFTextureAddressMode_NoTiling:
						texrec->u_mode = ETextureAddressMode_NoTiling;
						break;
					case EDFFTextureAddressMode_Wrap:
						texrec->u_mode = ETextureAddressMode_Wrap;
						break;
					case EDFFTextureAddressMode_Mirror:
						texrec->u_mode = ETextureAddressMode_Mirror;
						break;
					default:
					case EDFFTextureAddressMode_Clamp:
						texrec->u_mode = ETextureAddressMode_Clamp;
						break;
					case EDFFTextureAddressMode_Border:
						texrec->u_mode = ETextureAddressMode_Border;
						break;
					}
					switch (addressing_v) {
					case EDFFTextureAddressMode_NoTiling:
						texrec->v_mode = ETextureAddressMode_NoTiling;
						break;
					case EDFFTextureAddressMode_Wrap:
						texrec->v_mode = ETextureAddressMode_Wrap;
						break;
					case EDFFTextureAddressMode_Mirror:
						texrec->v_mode = ETextureAddressMode_Mirror;
						break;
					default:
					case EDFFTextureAddressMode_Clamp:
						texrec->v_mode = ETextureAddressMode_Clamp;
						break;
					case EDFFTextureAddressMode_Border:
						texrec->v_mode = ETextureAddressMode_Border;
						break;
					}
					printf("Filter flags 0x%04X Filter: %d %d\n", texrec->filter_flags, (addressing & 0xF0)>>4, addressing & 0x0F);
					fread(&texrec->unknown, sizeof(uint16_t), 1, fd);
					char str[256];

					fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd); //read string
					fread(&texrec->texturename, clumpHead.size, 1, fd);

					fread(&clumpHead, sizeof(DFFChunkInfo), 1, fd); //read string 2... only found it empty so far
					fread(&texrec->alphaname, clumpHead.size, 1, fd);

					printf("%s\n",texrec->texturename);
					printf("%s\n",texrec->alphaname);


				
					dff_out->last_material->textures.add(texrec);

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

void getMaterialFromRecord(MaterialRecord *matrec, CMaterial *mat, GeometryRecord *geom_rec) {
	float a = ((matrec->colour>>24)&0xff) / 255.0,b = ((matrec->colour>>16)&0xff) / 255.0,g= ((matrec->colour>>8)&0xff) / 255.0,r = (matrec->colour&0xff) / 255.0;
	uint8_t a_i = ((matrec->colour>>24)&0xff), b_i = ((matrec->colour>>16)&0xff),g_i= ((matrec->colour>>8)&0xff),r_i = (matrec->colour&0xff);
	mat->getDiffuseColour(r,g,b,a);
	char name[64];
	sprintf(name,"%p",mat);
	mat->setName(name);
	Core::Iterator<Core::Vector<TextureRecord *>, TextureRecord *> it = matrec->textures.begin();
	int level = 0;
	mat->setAmbientReflectionCoeff(matrec->ambient);
	mat->setDiffuseReflectionCoeff(matrec->diffuse);
	mat->setSpecularReflectionCoeff(matrec->specular);


	if(r_i == 60 && g_i == 255 && b_i == 0 && a_i == 255) { //primary colour
		const char *mat_type = "TYPE_PRIMARY_COLOUR";
		mat->setIdentifierChecksum(crc32(0, mat_type, strlen(mat_type)));
	} else if(r_i == 255 && g_i == 0 && b_i == 175 && a_i == 255) { //secondary colour
		const char *mat_type = "TYPE_SECONDARY_COLOUR";
		mat->setIdentifierChecksum(crc32(0, mat_type, strlen(mat_type)));
	}
	bool has_alpha = false;
	while(it != matrec->textures.end()) {
		TextureRecord *texrec = *it;
		if (strlen(texrec->alphaname) > 0) {
			has_alpha = true;
		}
		mat->setTextureName(texrec->texturename, level);
		mat->setBlendMode(EBlendMode_Modulate, level);
		mat->setTextureFilterMode(texrec->mat_filter_mode, level);
		mat->setTextureAddressMode(texrec->u_mode, texrec->v_mode, level++);
		it++;
	}
	if (has_alpha) {
		mat->setFlag(EMaterialFlag_HasTransparency);
	}
	else {
		mat->setFlag(EMaterialFlag_Opaque);
	}
}
CMesh *find_mesh_by_name_from_array(CMesh **meshes, int size, const char *name, DFFInfo *info) {
	for (int i = 0; i < size; i++) {
		if (strcmp(meshes[i]->getName(), name) == 0) {
			return meshes[i];
		}
	}
	return NULL;
}

FrameInfo *find_frame_by_bone_id(DFFInfo *info, uint32_t index) {
	std::vector<FrameInfo *>::iterator it = info->m_frames.begin();
	while(it != info->m_frames.end()) {
		FrameInfo *frame = *it;
		if(frame->bone_id == index) {
			return frame;
		}
		it++;
	}
	return NULL;
}
bool sort_bones_by_file_index(const DFFBone *bone1, const DFFBone *bone2) {
	return bone1->bone_file_index < bone2->bone_file_index;
}
void add_bones_from_dff(CMesh **meshes, uint32_t num_meshes, DFFInfo *info) {
	//sort bones by file index
	std::sort(info->m_bones.begin(), info->m_bones.end(), sort_bones_by_file_index);
	//map names
	for(int i=0;i<info->m_bones.size();i++) {
		DFFBone *dff_bone = info->m_bones[i];
		
		FrameInfo *frame = find_frame_by_bone_id(info,dff_bone->bone_id);
		
		sBone *bone_info = meshes[0]->getBone(i);

		bone_info->identifier.type = EDataType_String_ASCII;
		bone_info->identifier.sUnion.mString = (char *)&frame->name;
	}
	//map parents
	for(int i=0;i<info->m_bones.size();i++) {
		DFFBone *dff_bone = info->m_bones[i];
		FrameInfo *frame = find_frame_by_bone_id(info,dff_bone->bone_id);
		sBone *bone_info = meshes[0]->getBone(i);

		sBone *parent_bone = NULL;
		if(frame->parent_frame > 0) {
			FrameInfo *parent_frame = info->m_frames[frame->parent_frame];
			bone_info->parent = meshes[0]->getBoneByName(parent_frame->name);
		}
	}

}

DFFBone *get_dff_bone_by_index(DFFInfo *info, uint32_t index) {
	std::vector<DFFBone *>::iterator it = info->m_bones.begin();
	while(it != info->m_bones.end()) {
		DFFBone *bone = *it;
		if(bone->bone_number == index) {
			return bone;
		}
		it++;
	}
	return NULL;
}

void gta_dff_info_cleanup(DFFInfo *info) {
	std::vector<GeometryRecord *>::iterator it = info->m_geom_records.begin(); 
	while(it != info->m_geom_records.end()) {
		GeometryRecord *g = *it;
		if(g->vertex_data)
			free(g->vertex_data);
		if(g->normal_data) 
			free(g->normal_data);
		if(g->vertex_colours)
			free(g->vertex_colours);
		if(g->uv_data) {
			for(int i=0;i<g->uvcount;i++)
			{
				free(g->uv_data[i]);
			}
			free(g->uv_data);
		}
		if(g->indicies) {
			free(g->indicies);
		}
		if(g->weights)
			free(g->weights);
		if(g->bone_used)
			free(g->bone_used);
		if(g->bone_indices)
			free(g->bone_indices);
		for(int i=0;i<g->m_material_records.size();i++) {
			for(int j=0;j<g->m_material_records.size();j++) {
				delete g->m_material_records[i]->textures[j];
			}
			delete g->m_material_records[i];
		}
		delete g;
		it++;
	}
	for(int i=0;i<info->m_frames.size();i++) {
		FrameInfo *frame = info->m_frames[i];
		delete info->m_frames[i];
	}
	if(info->frame_map)
		free(info->frame_map);
}

bool gta_rw_import_dff(ImportOptions* impOpts) {
	FILE *fd = fopen(impOpts->path, "rb");
	DFFChunkInfo head;
	DFFInfo info;
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

	Core::Map<int, Core::Vector<glm::ivec3> > m_index_buffers;
	while(it != info.m_geom_records.end()) {
		GeometryRecord *rec = *it;

		num_materials += rec->m_material_records.size();
		num_verts += rec->vertex_count;
		int last_switchpoint = 0;
		uint16_t last_materialid = -1;
		for(int i=0;i<rec->face_count;i++) {
			glm::ivec4 indices = rec->indicies[i];
			rec->m_index_buffers[indices.w].add(glm::ivec3(indices.x, indices.y, indices.z));
		}
		it++;
	}

	CMesh **output_meshes = (CMesh**)malloc(info.m_geom_records.size() * sizeof(CMesh *));
	int mesh_id = 0;
	int num_meshes = 0;

	int mesh_vert_count = 0;
	
	int index_buffer_idx = 0, mesh_buffer_idx = 0;

	Core::Vector<CMaterial *> materials;

	Core::Map<int, int> m_mat_instance_counts;

	it = info.m_geom_records.begin();
	while(it != info.m_geom_records.end()) {
		GeometryRecord *g = *it;
		if(strstr(g->name, "dummy") || strstr(g->name, "_vlo") || strstr(g->name, "moving"))  {
			it++;
			continue;
		}
		output_meshes[mesh_buffer_idx] = new CMesh();
		output_meshes[mesh_buffer_idx]->setCoordinateSystem(ECoordinateSystem_Right);
		
		output_meshes[mesh_buffer_idx]->setUseIndexedMaterials(true);
		
		output_meshes[mesh_buffer_idx]->setName(g->name);
		output_meshes[mesh_buffer_idx]->setGroupId(crc32(0, g->name, strlen(g->name)));


		if(g->weights) {

			//skeleton info
			output_meshes[mesh_buffer_idx]->setNumBones(g->bone_count);
			for(int i=0;i<g->bone_count;i++) {
				DFFBone *dff_bone = get_dff_bone_by_index(&info, i);
				sBone *bone_info = output_meshes[mesh_buffer_idx]->getBone(i);
				memcpy(&bone_info->matrix, &dff_bone->matrix, sizeof(bone_info->matrix));				
			}
			//vertex bone weights
			output_meshes[mesh_buffer_idx]->setNumWeightSets(1);
			if(g->weights || g->bone_count) {
				output_meshes[mesh_buffer_idx]->setWeightsFloat(0, g->weights, g->vertex_count);
				output_meshes[mesh_buffer_idx]->setNumBoneIndexSets(1);
			}
			


			uint32_t *indices = (uint32_t *)malloc(sizeof(uint32_t) * g->vertex_count * 4);
			uint32_t *p = indices;
			uint8_t *x = g->bone_indices;
			for(int i=0;i<g->vertex_count;i++) {
				*p++ = *x++; //x
				*p++ = *x++; //y
				*p++ = *x++; //z
				*p++ = *x++;  //w
			}
			output_meshes[mesh_buffer_idx]->setBoneIndicesUInt32(0, indices, g->vertex_count * 4);
			free(indices);
			output_meshes[mesh_buffer_idx]->setWeightFlags(CMeshWeightTypeFlags_HasInverseBoneMatrices|CMeshWeightTypeFlags_HasBoneIndices);
			
			
		}
		
		
		g->output_mesh_id = mesh_buffer_idx;

		float *temp_verts = (float *)malloc(g->vertex_count * sizeof(float) * 3);
		float *temp_verts_p = temp_verts;
		Core::Map<int, int> found_materials; 
		for(int i=0;i<g->face_count;i++) {
			uint32_t matid = g->indicies[i].w;
			m_mat_instance_counts[matid]++;
		}

		output_meshes[mesh_buffer_idx]->setDefaultHierarchicalPosition(glm::value_ptr(g->default_hierarchical_position));
		output_meshes[mesh_buffer_idx]->setDefaultHierarchicalRotation(glm::value_ptr(g->default_hierarchical_rotation));

		output_meshes[mesh_buffer_idx]->setIndexLevels(g->m_index_buffers.size());
		Core::Iterator<Core::Map<int, Core::Vector<glm::ivec3> >, Core::MapItem< int, Core::Vector<glm::ivec3> >* > it2 = g->m_index_buffers.begin();
		int level = 0;
		
		while(it2 != g->m_index_buffers.end()) {
			Core::MapItem< int, Core::Vector<glm::ivec3> >* item = *it2;

			MaterialRecord *matrec = g->m_material_records[item->key];

			CMaterial *cmat = new CMaterial();
			getMaterialFromRecord(matrec, cmat, g);
			materials.add(cmat);
			output_meshes[mesh_buffer_idx]->setIndexMaterial(cmat, level);


			uint32_t *indices = (uint32_t*) malloc(sizeof(uint32_t) * 3 * item->value.size());
			uint32_t *p = indices;

			Core::Iterator<Core::Vector<glm::ivec3>, glm::ivec3> it3 = item->value.begin();
			int idx = 0;
			while(it3 != item->value.end()) {
				glm::ivec3 cur_indices = *it3;
				*p++ = cur_indices.x;
				*p++ = cur_indices.y;
				*p++ = cur_indices.z;
				it3++;
				idx++;
			}
			output_meshes[mesh_buffer_idx]->setIndices(indices, item->value.size() * 3, level);
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

		uint32_t *cols = (uint32_t*)temp_verts;
		if(g->vertex_colours) {
			for(int i=0;i<g->vertex_count;i++) {
				*cols++ = g->vertex_colours[i];
			}
			output_meshes[mesh_buffer_idx]->setColours((uint32_t*)temp_verts);
		}

		temp_verts_p = temp_verts;

		if(g->normal_data) {
			for(int i=0;i<g->vertex_count;i++) {
				memcpy(temp_verts_p,glm::value_ptr(g->normal_data[i]), sizeof(float) * 3);
				temp_verts_p += 3;
			}
			output_meshes[mesh_buffer_idx]->setNormals(temp_verts);
		}

		if(g->uv_data) {

			for(int i=0;i<g->uvcount;i++) {
				temp_verts_p = temp_verts;
				for(int j=0;j<g->vertex_count;j++) {
					memcpy(temp_verts_p,glm::value_ptr(g->uv_data[i][j]), sizeof(float) * 3);
					temp_verts_p += 3;
				}
				output_meshes[mesh_buffer_idx]->setUVWs(temp_verts, i);
			}
		}
		temp_verts_p = temp_verts;

		free(temp_verts);

		mesh_buffer_idx++;
		it++;
	}

	it = info.m_geom_records.begin();
	while (it != info.m_geom_records.end()) {
		GeometryRecord *g = *it;
		if (g->parent_frame != NULL) {
			CMesh *parent = find_mesh_by_name_from_array(output_meshes, mesh_buffer_idx, g->parent_frame->name, &info);
			if (parent) {
				output_meshes[g->output_mesh_id]->setParent(parent);
			}
		}
		it++;
	}	


	add_bones_from_dff(output_meshes, mesh_buffer_idx, &info);

	//run exporter
	ScenePack scene;
	memset(&scene,0,sizeof(scene));
	scene.m_meshes = (CMesh**)output_meshes;
	CMaterial **output_mats = (CMaterial**)malloc(materials.size() * sizeof(CMaterial*));
	Core::Iterator<Core::Vector<CMaterial *>, CMaterial*> matit = materials.begin();
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
	opts.type = ClassType_ScenePack;
	opts.srcPath = impOpts->path;
	opts.args = impOpts->expArgs;
	opts.path = impOpts->outpath;

	impOpts->exporter(&opts);

	gta_dff_info_cleanup(&info);
	for(int i=0;i<mesh_buffer_idx;i++) {
		delete output_meshes[i];
	}
	for(int i=0;i<scene.num_materials;i++) {
		delete scene.m_materials[i];
	}
	free(output_mats);
	free(meshes);
	free(output_meshes);
	
	return true;
}
bool gta_rw_export_dff(ExportOptions *expOpts) {
	return false;
}