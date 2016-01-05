#include "CTHPSCol.h"
#include <libthps/misc.h>

#include <Generic/CCollision.h>
#include <Generic/ScenePack.h>

#include <crc32.h>
#define COLLISION_SUB_INCH_PRECISION 16.0
#define COLLISION_RECIPROCAL_SUB_INCH_PRECISION 0.0625f

struct THPSColHeader
{
	int 	m_version;
	int 	m_num_objects;
	int 	m_total_num_verts;
	int 	m_total_num_faces_large;
	int 	m_total_num_faces_small;
	int		m_total_num_verts_large;
	int		m_total_num_verts_small;
	int		m_pad3;
};

typedef struct {
	uint32_t checksum;
	uint16_t flag_pad;
	uint32_t num_verts;
	uint32_t num_faces;
	uint8_t fixed_verts;
	uint8_t small_faces;

	float *out_verts;
	uint32_t *out_faces;

	float *vert_cursor;
	uint32_t *face_cursor;

	uint32_t faces_offset, verts_offset;

	float min[3];
	float max[3];

} ObjectInfo;

bool thps_xbx_import_col(ImportOptions* opts) {
	FILE *fd = fopen(opts->path, "rb");
	THPSColHeader head;
	fread(&head,sizeof(THPSColHeader), 1, fd);
	printf("Num Objs: %d\n",head.m_num_objects);
	printf("Num Verts: %d\n",head.m_total_num_verts);
	printf("Num Small Verts: %d\n",head.m_total_num_verts_small);
	printf("Num Large Verts: %d\n",head.m_total_num_verts_large);
	printf("Num Small Faces: %d\n",head.m_total_num_faces_small);
	printf("Num Large Faces: %d\n",head.m_total_num_faces_large);
	

	ObjectInfo *objs = (ObjectInfo *)malloc(head.m_num_objects*sizeof(ObjectInfo));
	memset(objs, 0, head.m_num_objects*sizeof(ObjectInfo));

	CCollision* mp_collision = new CCollision();


	for(int i=0;i<head.m_num_objects;i++) {
		uint32_t checksum;
		fread(&checksum, sizeof(uint32_t), 1, fd);
		uint16_t flags;
		fread(&flags, sizeof(uint16_t), 1, fd);

		uint16_t num_verts, num_faces;
		fread(&num_verts, sizeof(uint16_t), 1, fd);
		fread(&num_faces, sizeof(uint16_t), 1, fd);

		uint8_t small_faces;
		uint8_t fixed_verts;
		fread(&small_faces, sizeof(uint8_t), 1, fd);
		fread(&fixed_verts, sizeof(uint8_t), 1, fd);

		uint32_t face_offset;
		fread(&face_offset, sizeof(uint32_t), 1, fd);

		float bbox_min[4];
		float bbox_max[4];
		fread(&bbox_min,sizeof(float),4,fd);
		fread(&bbox_max,sizeof(float),4,fd);

		uint32_t verts_offset;
		fread(&verts_offset, sizeof(uint32_t), 1, fd);

		uint32_t node_offset;
		uint32_t master_vert_idx, pad;
		fread(&node_offset, sizeof(uint32_t), 1, fd);
		fread(&master_vert_idx, sizeof(uint32_t), 1, fd);
		fread(&pad, sizeof(uint32_t), 1, fd);
		
		printf("Checksum: %08X\n",checksum);
		printf("Num Verts: %d\n",num_verts);
		printf("Num faces: %d\n", num_faces);
		printf("Vert offset: %08X\n", verts_offset);
		printf("BBox: (%f, %f, %f) (%f, %f, %f)\n",bbox_min[0],bbox_min[1],bbox_min[2],bbox_max[0],bbox_max[1],bbox_max[2]);
		objs[i].small_faces = small_faces;
		objs[i].fixed_verts = fixed_verts;
		objs[i].num_faces = num_faces;
		objs[i].num_verts = num_verts;
		objs[i].checksum = checksum;

		memcpy(&objs[i].min, &bbox_min, sizeof(float)*3);
		memcpy(&objs[i].max, &bbox_max, sizeof(float)*3);

		objs[i].out_verts = (float *)malloc(objs[i].num_verts * sizeof(float) * 3);
		objs[i].out_faces = (uint32_t *)malloc(objs[i].num_faces * sizeof(uint32_t) * 3);
		objs[i].vert_cursor = objs[i].out_verts;
		objs[i].face_cursor = objs[i].out_faces;

		objs[i].verts_offset = verts_offset;
		objs[i].faces_offset = face_offset;
	}

	for(int i=0;i<head.m_num_objects;i++) {
		for(int j=0;j<objs[i].num_verts;j++) {
			if(objs[i].fixed_verts) {
				
				uint16_t pos[3];
				fread(&pos, sizeof(uint16_t), 3, fd);

				*objs[i].vert_cursor++ = (pos[0] * COLLISION_RECIPROCAL_SUB_INCH_PRECISION) + objs[i].min[0];
				*objs[i].vert_cursor++ = (pos[1] * COLLISION_RECIPROCAL_SUB_INCH_PRECISION) + objs[i].min[1];
				*objs[i].vert_cursor++ = (pos[2] * COLLISION_RECIPROCAL_SUB_INCH_PRECISION) + objs[i].min[2];
				//printf("Fixed Vert: %f %f %f\n", objs[i].vert_cursor[-3], objs[i].vert_cursor[-2], objs[i].vert_cursor[-1]);
				/*
				uint8_t intensity, pad;
				fread(&pad, sizeof(uint8_t), 1, fd);
				fread(&intensity, sizeof(uint8_t), 1, fd);
				printf("Intensity: %d\n",intensity);
				*/
			} else {
				float pos[3];
				fread(&pos, sizeof(float), 3, fd);
				*objs[i].vert_cursor++ = pos[0];
				*objs[i].vert_cursor++ = pos[1];
				*objs[i].vert_cursor++ = pos[2];

				//printf("Vert: %f %f %f\n", objs[i].vert_cursor[-3], objs[i].vert_cursor[-2], objs[i].vert_cursor[-1]);
			}			
		}
	}
	
	/*
	for(int i=0;i<head.m_num_objects;i++) {
		for(int j=0;j<objs[i].num_verts;j++) {
			if(objs[i].fixed_verts) {
				uint16_t pos[3];
				fread(&pos, sizeof(uint16_t), 3, fd);

				*objs[i].vert_cursor++ = pos[0] * COLLISION_RECIPROCAL_SUB_INCH_PRECISION + objs[i].min[0];
				*objs[i].vert_cursor++ = pos[1] * COLLISION_RECIPROCAL_SUB_INCH_PRECISION + objs[i].min[1];
				*objs[i].vert_cursor++ = pos[2] * COLLISION_RECIPROCAL_SUB_INCH_PRECISION + objs[i].min[2];
			}
			printf("Vert2: %f %f %f\n", objs[i].vert_cursor[-3], objs[i].vert_cursor[-2], objs[i].vert_cursor[-1]);
		}
	}*/
	
	//read vert cols/intensity
	for(int i=0;i<head.m_num_objects;i++) {
		for(int j=0;j<objs[i].num_verts;j++) {
			uint8_t intensity;
			fread(&intensity, sizeof(uint8_t), 1, fd);
			//printf("Intensity: %d\n",intensity);
		}
	}

	
	int pos = ftell(fd);
	while(pos & 3) {
		uint8_t pad;
		fread(&pad,sizeof(uint8_t), 1, fd);
		pos++;
	}
	//read faces
	for(int i=0;i<head.m_num_objects;i++) {
		for(int j=0;j<objs[i].num_faces;j++) {
			uint16_t flags;
			uint16_t terrain;
			fread(&flags, sizeof(uint16_t), 1, fd);
			fread(&terrain, sizeof(uint16_t), 1, fd);
			
			printf("Flags: %d terrain: %d\n",flags, terrain);
			if(objs[i].small_faces) {
				uint8_t faces[3], facepad;
				fread(&faces, sizeof(uint8_t), 3, fd);
				fread(&facepad, sizeof(uint8_t), 1, fd);
				*(objs[i].face_cursor++) = faces[0];
				*(objs[i].face_cursor++) = faces[1];
				*(objs[i].face_cursor++) = faces[2];
				printf("Face small: %d %d %d\n",faces[0],faces[1],faces[2]);
			} else {
				uint16_t faces[3];
				fread(&faces, sizeof(uint16_t), 3, fd);
				*(objs[i].face_cursor++) = faces[0];
				*(objs[i].face_cursor++) = faces[1];
				*(objs[i].face_cursor++) = faces[2];
				printf("Faces: %d %d %d\n",faces[0],faces[1],faces[2]);
			}
			if(objs[i].face_cursor[-1] > objs[i].num_verts || objs[i].face_cursor[-2] > objs[i].num_verts || objs[i].face_cursor[-3] > objs[i].num_verts) {
				printf("too big %08X\n", ftell(fd));
			}
		}
	}
	
	fclose(fd);


	ExportOptions exportopts;
	memset(&exportopts, 0, sizeof(exportopts));

	ScenePack scene;
	memset(&scene, 0, sizeof(ScenePack));

	
	for(int i=0;i<head.m_num_objects;i++) {
		if(objs[i].num_faces == 0 || objs[i].num_verts == 0) continue;

		COLTriangleMesh mesh;
		memset(&mesh,0,sizeof(mesh));
		mesh.num_indices = objs[i].num_faces;
		mesh.num_verts = objs[i].num_verts;
		mesh.indices = objs[i].out_faces;
		mesh.verticies = objs[i].out_verts;
		mesh.checksum = objs[i].checksum;
		mp_collision->addTriMesh(mesh);
		//mp_collision->addChild(col);
	}

	mp_collision->setCoordinateSystem(ECoordinateSystem_Left_XZY);
	mp_collision->setChecksum(crc32(0, "SC2", 3));
	scene.m_collision = mp_collision;
	exportopts.dataClass = &scene;
	exportopts.path = opts->outpath;
	opts->exporter(&exportopts);
	return false;
}
bool thps_xbx_export_col(ExportOptions* opts) {
	return false;
}