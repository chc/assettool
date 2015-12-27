#include "plyloader.h"
#include "rply.h"
#include <Generic/CMesh.h>
#include <glm/glm.hpp>

#include <Generic/ScenePack.h>
typedef struct {
	glm::vec3 *verticies;
	int vert_index;
	glm::vec3 *normals;
	int normals_index;
	glm::uvec3 *indicies;
	int faces_index;
} PlyCBInfo;


static int read_vertex_cb(p_ply_argument argument) {
	long idx;
	PlyCBInfo *cbinfo;

	ply_get_argument_user_data(argument, (void **)&cbinfo, &idx);

	glm::vec3 *vec = &cbinfo->verticies[cbinfo->vert_index];
	
	switch (idx) {
	case 0: //x
		vec->x = ply_get_argument_value(argument);
		break;
	case 1: //y
		vec->y = ply_get_argument_value(argument);
		break;
	case 2: //z
		vec->z = ply_get_argument_value(argument);
		cbinfo->vert_index++;
		break;
	}

	return 1;
}
static int read_normal_cb(p_ply_argument argument) {
	long idx;
	PlyCBInfo *cbinfo;
	ply_get_argument_user_data(argument, (void **)&cbinfo, &idx);

	glm::vec3 *vec = &cbinfo->normals[cbinfo->normals_index];

	switch (idx) {
	case 0: //x
		vec->x = ply_get_argument_value(argument);
		break;
	case 1: //y
		vec->y = ply_get_argument_value(argument);
		break;
	case 2: //z
		vec->z = ply_get_argument_value(argument);
		cbinfo->normals_index++;
		break;
	}
	return 1;
}
static int read_face_cb(p_ply_argument argument) {
	long length, value_index;
	ply_get_argument_property(argument, NULL, &length, &value_index);

	PlyCBInfo *cbinfo;
	ply_get_argument_user_data(argument, (void **)&cbinfo, NULL);

	glm::uvec3 *vec = &cbinfo->indicies[cbinfo->faces_index];

	switch (value_index) {
	case 0: //x
		vec->x = ply_get_argument_value(argument);
		break;
	case 1: //y
		vec->y = ply_get_argument_value(argument);
		break;
	case 2: //z
		vec->z = ply_get_argument_value(argument);
		cbinfo->faces_index++;
		break;
	}
	return 1;
}
bool ply_import_mesh(ImportOptions* opts) {
	PlyCBInfo cb_info;
	memset(&cb_info, 0, sizeof(cb_info));

	p_ply ply = ply_open(opts->path, NULL, 0, NULL);
	if (!ply) return false;
	if (!ply_read_header(ply)) return false;

	long nvertices = ply_set_read_cb(ply, "vertex", "x", read_vertex_cb, &cb_info, 0);
	ply_set_read_cb(ply, "vertex", "y", read_vertex_cb, &cb_info, 1);
	ply_set_read_cb(ply, "vertex", "z", read_vertex_cb, &cb_info, 2);

	//if has normals
	long nnormals = ply_set_read_cb(ply, "vertex", "nx", read_normal_cb, &cb_info, 0);
	ply_set_read_cb(ply, "vertex", "ny", read_normal_cb, &cb_info, 1);
	ply_set_read_cb(ply, "vertex", "nz", read_normal_cb, &cb_info, 2);

	long nfaces = ply_set_read_cb(ply, "face", "vertex_indices", read_face_cb, &cb_info, 0);

	if (nvertices > 0) {
		cb_info.verticies = new glm::vec3[nvertices];
	}
	if (nnormals > 0) {
		cb_info.normals = new glm::vec3[nnormals];
	}
	if (nfaces > 0) {
		cb_info.indicies = new glm::uvec3[nfaces];
	}

	if (!ply_read(ply)) return false;

	CMesh *mesh = new CMesh();
	mesh->setNumVerts(nvertices);
	float *verts = (float *)malloc(nvertices * sizeof(float) * 3);
	for(int i=0, o = 0;i<nvertices;i++,o+=3) {
		verts[o + 0] = cb_info.verticies[i].x;
		verts[o + 1] = cb_info.verticies[i].y;
		verts[o + 2] = cb_info.verticies[i].z;
	}
	mesh->setVerticies(verts);
	if(nnormals > 0) {
		for(int i=0, o = 0;i<nvertices;i++,o+=3) {
			verts[o + 0] = cb_info.normals[i].x;
			verts[o + 1] = cb_info.normals[i].y;
			verts[o + 2] = cb_info.normals[i].z;
		}
		mesh->setNormals(verts);
	}

	uint32_t *faces = (uint32_t *)malloc(nfaces * sizeof(uint32_t) * 3);

	for(int i=0, o = 0;i<nfaces;i++, o += 3) {
		faces[o + 0] = cb_info.indicies[i].x;
		faces[o + 1] = cb_info.indicies[i].y;
		faces[o + 2] = cb_info.indicies[i].z;
	}

	mesh->setIndices(faces,nfaces * 3);

	free(verts);
	free(faces);


	//run exporter
	ScenePack scene;
	memset(&scene,0,sizeof(scene));
	scene.m_meshes = (CMesh**)&mesh;
	scene.m_materials = (CMaterial**)NULL;
	scene.num_meshes = 1;
	scene.num_materials = 0;
	


	ExportOptions expOpts;
	memset(&expOpts,0,sizeof(expOpts));
	expOpts.path = opts->outpath;
	expOpts.srcPath = opts->path;
	expOpts.dataClass = (void *)&scene;
	expOpts.args = opts->expArgs;
	opts->exporter(&expOpts);

	delete mesh;

	ply_close(ply);
	return true;
}
bool ply_export_mesh(ExportOptions* opts) {
	return false;
}