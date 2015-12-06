#include "main.h"
#include "col.h"

#include "ScenePack.h"
#include "CCollision.h"
#include "crc32.h"
#include <glm/glm.hpp>
#define GTA_COL3_MAGIC 0x334c4f43
bool gta_rw_export_col(ExportOptions *expOpts) {
	return false;
}
typedef struct {
	float x;
	float y;
	float z;
}GTACOLVector;
typedef struct {
	GTACOLVector min;
	GTACOLVector max;
	GTACOLVector center;
	float radius;
} GTACOLBounds;
typedef struct {
	uint8_t material;
	uint8_t flags;
	uint8_t unknown;
	uint8_t light;
} GTACOLSurface;

typedef struct{
	GTACOLVector center;
	float radius;
	GTACOLSurface surface;
} GTACOLSphere;

//col 3 header only
typedef struct {
	uint32_t magic;
	uint32_t file_size;
	char model_name[22];
	uint16_t objid;
	GTACOLBounds bounds;

	//v2 stuff
	uint16_t num_spheres;
	uint16_t num_boxes;
	uint16_t num_mesh_faces;
	uint8_t num_wheels;
	uint8_t padding; //or part of wheels..?
	uint32_t flags;
	uint32_t sphere_offset;
	uint32_t box_offset;
	uint32_t suspenison_lines_offset;
	uint32_t mesh_vertices_offset;
	uint32_t mesh_faces_offset;
	uint32_t triangle_planes_offset;

	//v3 stuff
	uint32_t num_shade_faces;
	uint32_t shadow_vertices_offset;
	uint32_t shadow_faces_offset;
} GTACOLHeader;


void dump_collision_meshes(FILE *fd, GTACOLHeader *head, int offset, CCollision *m_out_collision) {
	fseek(fd, head->mesh_faces_offset + offset, SEEK_SET);
	uint16_t largest_index = 0;
	uint32_t *indices = (uint32_t *)malloc(head->num_mesh_faces * 3 * sizeof(uint32_t));
	uint32_t *p = indices;
	for (int i = 0; i < head->num_mesh_faces; i++) {
		uint16_t x, y, z;
		uint8_t material, light;
		fread(&x, sizeof(uint16_t), 1, fd);
		fread(&y, sizeof(uint16_t), 1, fd);
		fread(&z, sizeof(uint16_t), 1, fd);

		if (x > largest_index) {
			largest_index = x;
		}
		if (y > largest_index) {
			largest_index = y;
		}
		if (z > largest_index) {
			largest_index = z ;
		}
		*p++ = x;
		*p++ = y;
		*p++ = z;

		fread(&material, sizeof(uint8_t), 1, fd);
		fread(&light, sizeof(uint8_t), 1, fd);

		printf("%d %d %d %d %d\n", x, y, z, material, light);

	}
	
	fseek(fd, head->mesh_vertices_offset + offset, SEEK_SET);
	//largest_index = (floor(largest_index / 3.0));
	//largest_index -= 2;
	printf("%d is largest index\n", largest_index);


	uint32_t num_verts = head->num_mesh_faces == 0 ? 0 : largest_index + 1;
	float *verts = (float *)malloc(sizeof(float) * 3 * num_verts);
	float *v = verts;
	for (int i = 0; i < num_verts; i++) {
		int16_t x, y, z;
		fread(&x, sizeof(int16_t), 1, fd);
		fread(&y, sizeof(int16_t), 1, fd);
		fread(&z, sizeof(int16_t), 1, fd);
		*v++ = (x / 128.0f);
		*v++ = (y / 128.0f);
		*v++ = (z / 128.0f);

		printf("%f %f %f\n", (x / 128.0f), (y / 128.0f), (z/128.0f));
	}

	COLTriangleMesh mesh;
	mesh.indices = indices;
	mesh.verticies = verts;
	mesh.num_indices = head->num_mesh_faces;
	mesh.num_verts = num_verts;

	m_out_collision->addTriMesh(mesh);
}
void dump_gta_collision_spheres(FILE *fd, GTACOLHeader *head, int offset, CCollision *m_out_collision) {
	fseek(fd, head->sphere_offset + offset, SEEK_SET);
	for (int i = 0; i < head->num_spheres; i++) {
		GTACOLVector center;
		float radius;
		GTACOLSurface surface;
		fread(&center, sizeof(float), 3, fd);
		fread(&radius, sizeof(float), 1, fd);
		fread(&surface, sizeof(GTACOLSurface), 1, fd);
		COLSphere sphere;
		sphere.radius = radius;
		memcpy(&sphere.center, &center, sizeof(float) * 3);
		m_out_collision->addSphere(sphere);
	}
}
void dump_gta_bboxes(FILE *fd, GTACOLHeader *head, int offset, CCollision *m_out_collision) {
	fseek(fd, head->box_offset + offset, SEEK_SET);
	for (int i = 0; i < head->num_boxes; i++) {
		GTACOLVector begin, end;
		GTACOLSurface surface;
		fread(&begin, sizeof(GTACOLVector), 1, fd);
		fread(&end, sizeof(GTACOLVector), 1, fd);
		fread(&surface, sizeof(surface), 1, fd);
		printf("B: %f %f %f\n", begin.x, begin.y, begin.z);
		printf("E: %f %f %f\n", end.x, end.y, end.z);

		COLBBox box;
		memcpy(&box.min, &begin, sizeof(GTACOLVector));
		memcpy(&box.max, &end, sizeof(GTACOLVector));
		m_out_collision->addBBOX(box);
	}
}
bool gta_rw_import_col(ImportOptions* opts) {
	FILE *fd = fopen(opts->path, "rb");
	GTACOLHeader head;
	CCollision *m_out_collision = new CCollision();
	do {
		
		int head_pos = ftell(fd);
		fread(&head, sizeof(head), 1, fd);
		if (head.magic != GTA_COL3_MAGIC)
			break;

		CCollision *col = new CCollision();
		
		dump_collision_meshes(fd, &head, head_pos + sizeof(uint32_t), col);
		dump_gta_bboxes(fd, &head, head_pos + sizeof(uint32_t), col);
		dump_gta_collision_spheres(fd, &head, head_pos + sizeof(uint32_t), col);
		printf("%s\n", head.model_name);
		fseek(fd, head_pos, SEEK_SET);
		fseek(fd, head.file_size + (sizeof(uint32_t) * 2), SEEK_CUR);
		col->setChecksum(crc32(0, head.model_name, strlen(head.model_name)));
		printf("Col checksum: %08X\n", col->getChecksum());
		m_out_collision->addChild(col);
	} while (head.magic == GTA_COL3_MAGIC && !feof(fd));
 	fclose(fd);


	ExportOptions exportopts;
	memset(&exportopts, 0, sizeof(exportopts));

	ScenePack scene;
	memset(&scene, 0, sizeof(ScenePack));


	m_out_collision->setChecksum(crc32(0, opts->outpath, strlen(opts->outpath)));
	scene.m_collision = m_out_collision;
	exportopts.dataClass = &scene;
	exportopts.path = opts->outpath;
	opts->exporter(&exportopts);
	return false;
}