#include <Generic/CMesh.h>
#include <Generic/CMaterial.h>
#include <Assimp/AssImpModel.h>
#include <Generic/ScenePack.h>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing fla


void import_assimp_mesh(aiMesh *mesh, CMesh *out) {
	int num_verts = mesh->mNumVertices;
	out->setNumVerts(num_verts);

	float *vert_buf = (float *)malloc(sizeof(float)*num_verts * 3);
	float *p = vert_buf;
	for (int i = 0; i < num_verts; i++) {
		aiVector3D &vec = mesh->mVertices[i];
		*p++ = vec.x;
		*p++ = vec.y;
		*p++ = vec.z;
	}
	out->setVerticies(vert_buf);
	p = vert_buf;
	if (mesh->HasNormals()) {
		for (int i = 0; i < num_verts; i++) {
			aiVector3D &vec = mesh->mNormals[i];
			*p++ = vec.x;
			*p++ = vec.y;
			*p++ = vec.z;
		}
	}
	int num_colours = mesh->GetNumColorChannels();
	p = vert_buf;
	if (mesh->HasVertexColors(0)) {
		uint32_t *cols = (uint32_t*)vert_buf;
		for (int i = 0; i < num_verts; i++) {
			aiColor4D *col = mesh->mColors[i];
			uint8_t r_c = (uint8_t)(col->r * 255.0);
			uint8_t g_c = (uint8_t)(col->g * 255.0);
			uint8_t b_c = (uint8_t)(col->b * 255.0);
			uint8_t a_c = (uint8_t)(col->a * 255.0);
			uint32_t i_col = r_c & 0xff | (g_c & 0xff) << 8 | (b_c & 0xff) << 16 | (a_c & 0xff) << 24;
			cols[i] = i_col;
		}
		out->setColours((uint32_t*)vert_buf);
	}
	int num_uvs = mesh->GetNumUVChannels();
	
	for (int i = 0; i < num_uvs; i++) {
		p = vert_buf;
		aiVector3D *vec = mesh->mTextureCoords[i];
		for (int j = 0; j < num_verts; j++) {
			*p++ = vec[j].x;
			*p++ = vec[j].y;
			*p++ = vec[j].z;
		}
		out->setUVWs(vert_buf, i);
	}
	uint32_t *indices = (uint32_t*)malloc(mesh->mNumFaces * sizeof(uint32_t) * 3); //TODO: use num indices
	memset(indices, 0, mesh->mNumFaces * sizeof(uint32_t) * 3);
	uint32_t *ip = indices;
	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace *face = &mesh->mFaces[i];
		uint32_t *index_ptr = face->mIndices;
		for (int j = 0; j < face->mNumIndices; j++) {
			*ip++ = *index_ptr++;
		}
	}

	char name[128];
	sprintf(name, "%p", mesh);
	out->setName(name);
	out->setIndices(indices, mesh->mNumFaces * 3);
	free(indices);
}
bool assimp_import_model(ImportOptions* impOpts) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(impOpts->path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (!scene)
	{
	
		return false;
	}
	CMesh **output_meshes = (CMesh **)malloc(sizeof(CMesh *) * scene->mNumMeshes);
	for (int i = 0; i < scene->mNumMeshes; i++) {
		CMesh *mesh = new CMesh();
		aiMesh *ai_mesh = scene->mMeshes[i];
		import_assimp_mesh(ai_mesh, mesh);
		output_meshes[i] = mesh;
	}

	ScenePack pack;
	memset(&pack, 0, sizeof(ScenePack));
	pack.m_meshes = output_meshes;
	pack.num_meshes = scene->mNumMeshes;

	ExportOptions opts;
	memset(&opts, 0, sizeof(opts));

	opts.dataClass = &pack;
	opts.srcPath = impOpts->path;
	opts.args = impOpts->expArgs;
	opts.path = impOpts->outpath;
	impOpts->exporter(&opts);
	return false;
}
bool assimp_export_model(ExportOptions* opts) {
	return false;
}