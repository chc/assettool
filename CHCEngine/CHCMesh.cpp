#include <CHCEngine/CCHCEngine.h>
#include <CHCEngine/CHCMesh.h>
#include <Generic/CMesh.h>
#include <crc32.h>
#include <Generic/ScenePack.h>
#include <Generic/CImage.h>
#include <algorithm>
#include <CHCEngine/CHCTexture.h>
#include <Generic/CTextureCollection.h>
#include <Generic/CCollision.h>
#include <Generic/BSP/BSP.h>
#include <Generic/BSP/BSPGen.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define CHCMESH_VERSION 4
#define CHC_SKELETON_VERSION 3
enum ECHCMeshFlags { //must corrospond to game
	ECHCMeshFlag_ColAsInt = (1<<0),
	ECHCMeshFlag_HasNormals = (1<<1),
	ECHCMeshFlag_HasCol = (1<<2),
	ECHCMeshFlag_HasUVs = (1<<3),
	ECHCMeshFlag_MaterialIndexID = (1<<4),
	ECHCMeshFlag_HasDefaultHiearchyPos = (1<<5),
	ECHCMeshFlag_HasDefaultHiearchyRot = (1 << 6),
	ECHCMeshFlag_HasBBOX = (1 << 7),
	ECHCMeshFlag_HasWeights_1W4I = (1 << 8),
};
enum ECHCPrimType { //must corrospond to game
	ECHCPrimType_TriangleList,
	ECHCPrimType_TriangleStrips,
};
bool chc_engine_import_mesh(ImportOptions* opts) {
	return false;
}

void write_mesh(CMesh *mesh, FILE* fd) {
	mesh->convertToCoordinateSystem(ECoordinateSystem_Left_XZY);
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
			float r = ((colour>>24)&0xff) / 255.0,g = ((colour>>16)&0xff) / 255.0,b = ((colour>>8)&0xff) / 255.0,a = (colour&0xff) / 255.0;
			*p++ = r;
			*p++ = g;
			*p++ = b;
			*p++ = a;
		}
	}
	float *uvs = mesh->getUVWs(0);

	uint32_t flags = 0;

	CMaterial *mat;

	uint32_t num_uv_sets = mesh->getUVLayers();

	uint32_t stride = sizeof(float) * 3;

	int num_indices, num_weights;
	uint32_t *bone_indices_u32 = mesh->getBoneIndicesUInt32(0, num_indices);
	uint32_t *vertex_weights_uint32 = mesh->getWeightsUInt32(0, num_weights);

	//TODO: assert bone indices = vert count * 4, weights = vert count

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
		if (num_uv_sets == 0) {
			num_uv_sets = 1;
		}
		stride += (sizeof(float) * 3) * num_uv_sets;
	}
	if(vertex_weights_uint32 && bone_indices_u32) {
		stride += sizeof(uint32_t); //vertex weights
		stride += sizeof(uint32_t) * 4; //bone indices
		flags |= ECHCMeshFlag_HasWeights_1W4I;
	}

	if(mesh->getUseIndexedMaterials()) {
		flags |= ECHCMeshFlag_MaterialIndexID;
	}
	float *default_hiearchy_pos = mesh->getDefaultHierarchialPosition();
	float *default_hiearchy_rot = mesh->getDefaultHiearchialRotation();
	
	if(default_hiearchy_pos) {
		if(!(default_hiearchy_pos[0] = 0.0 && default_hiearchy_pos[1] == 0.0 && default_hiearchy_pos[2] == 0.0)) {
			flags |= ECHCMeshFlag_HasDefaultHiearchyPos;
		}
	}

	COLBBox bbox = mesh->getBBox();
	if (bbox.checksum != 0) {
		flags |= ECHCMeshFlag_HasBBOX;
	}

	if(default_hiearchy_rot) {
		for(int i=0;i<9;i++) {
			if(default_hiearchy_rot[i] != 0.0) {
				flags |= ECHCMeshFlag_HasDefaultHiearchyRot;
			}
		}
	}

	fwrite(&flags,sizeof(uint32_t),1,fd);
	fwrite(&stride,sizeof(uint32_t),1,fd);
	fwrite(&num_uv_sets, sizeof(uint32_t), 1, fd);
	uint32_t num_materials = mesh->getNumMaterials();
	if((num_materials == -1 || !mesh->getUseIndexedMaterials())) {
		num_materials = 1;
		fwrite(&num_materials, sizeof(uint32_t), 1, fd);
		uint32_t material_checksum = 0;

		CMaterial *mat = mesh->getMaterial();
		if(mat->getIdentifierChecksum() == 0) {
			material_checksum = crc32(0,mat->getName(),strlen(mat->getName()));
		} else {
			material_checksum = mat->getIdentifierChecksum();
		}

		fwrite(&material_checksum,sizeof(uint32_t),1,fd);
	} else {
		fwrite(&num_materials, sizeof(uint32_t), 1, fd);
		for(int i=0;i<num_materials;i++) {
			mat = mesh->getIndexMaterial(i);
			uint32_t k = 0;
			if(mat == NULL) {
				fwrite(&k, sizeof(uint32_t), 1, fd);
			}
			else {
				if(mat->getIdentifierChecksum() == 0) {
					k = crc32(0, mat->getName(), strlen(mat->getName()));
				} else {
					k = mat->getIdentifierChecksum();
				}
				
				fwrite(&k, sizeof(uint32_t), 1, fd);
			}			
		}
	}
	

	uint32_t group_checksum = mesh->getGroupId();
	fwrite(&group_checksum, sizeof(uint32_t), 1, fd);

	uint32_t parent_checksum = 0;
	if (mesh->getParent() != NULL) {
		parent_checksum = mesh->getParent()->getGroupId();
	}
	fwrite(&parent_checksum, sizeof(uint32_t), 1, fd);

	if(flags & ECHCMeshFlag_HasDefaultHiearchyPos) {
		fwrite(mesh->getDefaultHierarchialPosition(), sizeof(float), 3, fd);
	}
	if(flags & ECHCMeshFlag_HasDefaultHiearchyRot)  {
		fwrite(mesh->getDefaultHiearchialRotation(), sizeof(float), 9, fd);
	}

	if (flags & ECHCMeshFlag_HasBBOX) {
		fwrite(&bbox.min, sizeof(float), 3, fd);
		fwrite(&bbox.max, sizeof(float), 3, fd);
	}
	float *uv_sets[MAX_MESH_TEXTURES];
	memset(&uv_sets,0,sizeof(uv_sets));
	for(int i=0;i<MAX_MESH_TEXTURES;i++) {
		uv_sets[i] = mesh->getUVWs(i);
	}
	for(uint32_t i=0;i<num_verts;i++) {
		fwrite(verts,sizeof(float),3,fd);
		verts += 4;
		if(float_colours != NULL) {
			fwrite(float_colours,sizeof(float),4,fd);
			float_colours += 4;
		}
		if(normals != NULL) {
			fwrite(normals,sizeof(float),3,fd);
			normals +=  4;
		}
		for(int j=0;j<num_uv_sets;j++) {
			if(uv_sets[j] != NULL) {
				fwrite(uv_sets[j],sizeof(float),3,fd);
				uv_sets[j] += 4;
			}
		}
		if(flags & ECHCMeshFlag_HasWeights_1W4I) {
			fwrite(vertex_weights_uint32, sizeof(uint32_t), 1, fd);
			fwrite(bone_indices_u32, sizeof(uint32_t), 4, fd);
			//printf("Writing: %d (%d,%d,%d,%d)\n",vertex_weights_uint32[0],bone_indices_u32[0],bone_indices_u32[1],bone_indices_u32[2],bone_indices_u32[3]);
			vertex_weights_uint32++;
			bone_indices_u32 += 4;

		}

	}
	int num_index_sets = mesh->getNumIndexLevels();
	
	if(num_index_sets == 0) {
		num_index_sets = 1;
		fwrite(&num_index_sets, sizeof(uint32_t), 1, fd);
		fwrite(&num_indicies,sizeof(uint32_t),1,fd);
		fwrite(mesh->getIndices(),sizeof(uint32_t),num_indicies,fd);
	} else {
		fwrite(&num_index_sets, sizeof(uint32_t), 1, fd);
		for(int i=0;i<num_index_sets;i++) {
			uint32_t* indices =  mesh->getIndices(i);
			num_indicies = mesh->getNumIndicies(i);
			fwrite(&num_indicies,sizeof(uint32_t),1,fd);
			fwrite(indices, sizeof(uint32_t), num_indicies, fd);
		}
	}
}

void write_material(CMaterial *material, FILE* fd) {
	uint32_t checksum = material->getIdentifierChecksum();
	if(checksum == 0) {
		checksum = crc32(0,material->getName(),strlen(material->getName()));
	}
	printf("Writing Mat: %08X %s\n", checksum, material->getName());
	fwrite(&checksum,sizeof(uint32_t),1,fd);

	checksum = material->getIdentifierChecksum();
	fwrite(&checksum, sizeof(uint32_t), 1, fd);

	uint32_t flags = (uint32_t)material->getFlags();
	fwrite(&flags, sizeof(uint32_t), 1, fd);

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
	

	uint32_t tex_count = 0;
	while((material->getTexture(tex_count) != NULL || material->getTextureChecksum(tex_count) != 0) && tex_count < MAX_MATERIAL_TEXTURES) {
		tex_count++;
	}
	int i = 0;
	fwrite(&tex_count,sizeof(uint32_t),1,fd);
	while(tex_count--) {
		tex = material->getTexture(i);
		if (tex != NULL) {
			checksum = crc32(0, tex->getPath(), strlen(tex->getPath()));
			fwrite(&checksum, sizeof(uint32_t), 1, fd);
			tex->getOffset(col[0], col[1]);
			fwrite(&col, sizeof(float), 2, fd);
		}
		else {
			checksum = material->getTextureChecksum(i);
			fwrite(&checksum, sizeof(uint32_t), 1, fd);
			col[0] = 0.0;
			col[1] = 0.0;
			fwrite(&col, sizeof(float), 2, fd);
		}

		if(material->getFlags() & EMaterialFlag_HasDiffuseColour) {
			material->getDiffuseColour(col[0], col[1], col[2], col[3], i);
			fwrite(&col,sizeof(float),4,fd);
		}
	
		uint8_t filter_mode = (uint8_t)material->getTextureFilterMode(i);
		ETextureAddresingMode u_mode, v_mode;
		material->getTextureAddressModes(u_mode, v_mode, i);
		uint8_t u, v;
		u = (uint8_t)u_mode;
		v = (uint8_t)v_mode;
		fwrite(&filter_mode, sizeof(uint8_t), 1, fd);
		fwrite(&u, sizeof(uint8_t), 1, fd);
		fwrite(&v, sizeof(uint8_t), 1, fd);
		i++;
	}
	if(flags & EMaterialFlag_HasShaderCode) {
		uint32_t code_len = 0;
		uint8_t *buffer = material->getShaderCode(&code_len);
		fwrite(&code_len, sizeof(uint32_t), 1, fd);
		fwrite(buffer,code_len,1,fd);
	}
}

void write_collision(FILE *fd, CCollision *collision) {
	uint32_t version = CHCMESH_VERSION;
	fwrite(&version,sizeof(uint32_t),1,fd);

	collision->convertToCoordinateSystem(ECoordinateSystem_Right);

	uint32_t checksum = collision->getChecksum();
	fwrite(&checksum, sizeof(uint32_t), 1, fd);

	uint32_t num_children = collision->getChildren().size();
	fwrite(&num_children, sizeof(uint32_t), 1, fd);
	
	std::vector<COLBBox> boxes = collision->getBBoxes();
	uint32_t num_bboxes = boxes.size();
	fwrite(&num_bboxes, sizeof(uint32_t), 1, fd);
	std::vector<COLBBox>::iterator it = boxes.begin();
	while(it != boxes.end()) {
		COLBBox box = *it;
		fwrite(&box.checksum, sizeof(uint32_t), 1, fd);
		fwrite(&box.min, sizeof(float), 3, fd);
		fwrite(&box.max, sizeof(float), 3, fd);
		it++;
	}

	std::vector<COLTriangleMesh> tri_meshes = collision->getTriMeshes();
	std::vector<COLTriangleMesh>::iterator tri_it = tri_meshes.begin();

	uint32_t num_triangle_meshes = tri_meshes.size();
	fwrite(&num_triangle_meshes, sizeof(uint32_t), 1, fd);

	while (tri_it != tri_meshes.end()) {
		COLTriangleMesh mesh = *tri_it;
		fwrite(&mesh.num_indices, sizeof(uint32_t), 1, fd);
		uint32_t *p = mesh.indices;
		for (int i = 0; i < mesh.num_indices; i++) {
			uint32_t x;
			x = *p++;
			fwrite(&x, sizeof(uint32_t), 1, fd);
		}
		fwrite(&mesh.num_verts, sizeof(uint32_t), 1, fd);
		float *v = mesh.verticies;
		for (int i = 0; i < mesh.num_verts; i++) {
			float x, y, z;
			x = *v++;
			y = *v++;
			z = *v++;
			fwrite(&x, sizeof(float), 1, fd);
			fwrite(&y, sizeof(float), 1, fd);
			fwrite(&z, sizeof(float), 1, fd);
		}
		tri_it++;
	}

	std::vector<COLSphere> spheres = collision->getSpheres();
	uint32_t num_spheres = spheres.size();
	std::vector<COLSphere>::iterator its = spheres.begin();
	fwrite(&num_spheres, sizeof(uint32_t), 1, fd);
	while (its != spheres.end()) {
		COLSphere sphere = *its;
		fwrite(&sphere.center, sizeof(float), 3, fd);
		fwrite(&sphere.radius, sizeof(float), 1, fd);
		its++;
	}

	std::vector<CCollision *> children = collision->getChildren();
	std::vector<CCollision *>::iterator it2 = children.begin();
	while (it2 != children.end()) {
		write_collision(fd, *it2);
		it2++;
	}
}
void write_skeleton(CMesh *mesh, FILE *fd) {
	/*
	uint32_t bone_count = mesh->getNumBoneIndexSets();
	uint32_t num_bone_entries;
	DataMapEntry *mp_entries = mesh->getBoneNameMap(num_bone_entries);
	uint32_t checksum, parent_checksum;

	uint32_t num_indexs;
	uint32_t *parents = mesh->getBoneParentIDs(num_indexs);

	fwrite(&bone_count, sizeof(uint32_t), 1, fd);
	//ids
	for(int i=0;i<bone_count;i++) {
		const char *str = (const char *)mp_entries[i].value;
		checksum = crc32_extended(str, strlen(str));
		fwrite(&checksum, sizeof(uint32_t), 1, fd);


		printf("Write Bone: %s : 0x%08x 0x%08x\n",str, checksum, parent_checksum);
	}
	//parent ids
	for(int i=0;i<bone_count;i++) {
		if(parents[i] != -1) {
			//printf("aaa %d\n",i);
			printf("bbb %d\n",parents[i]);
			printf("Bones: %d %d\n",parents[i],parents[i],mp_entries[parents[i]].identifier);
			//printf("Len: %s\n",(mp_entries[parents[i]].value));
			//checksum = crc32_extended(mp_entries[parents[i]].value, strlen(mp_entries[parents[i]].value));	
		} else {
			checksum = 0;
		}
		printf("Writing parent checksum: %s %08X\n", mp_entries[i].value, checksum);
		fwrite(&checksum, sizeof(uint32_t), 1, fd);
	}

	//flipped anims
	for(int i=0;i<bone_count;i++) {
		checksum = 0;
		fwrite(&checksum, sizeof(uint32_t), 1, fd);
	}

	for(int i=0;i<bone_count;i++) {
		float *_matrix = mesh->getInverseBoneMatrices(i);

		glm::mat4 matrix = glm::make_mat4(_matrix);
		glm::quat quat = glm::quat_cast(matrix);
		printf("Bone  %d\n",i);

		printf("(%f, %f, %f, %f)\n",quat.x,quat.y,quat.z,quat.w);
		printf("(%f, %f, %f, %f)\n",_matrix[12 + 0],_matrix[12 + 1],_matrix[12 + 2],_matrix[12 +3]);
		fwrite(glm::value_ptr(quat), sizeof(float), 4, fd);
		fwrite(&_matrix[12], sizeof(float), 4, fd);
	}
	*/
}
bool chc_engine_export_mesh(ExportOptions* opts) {
	ScenePack *scenepack = (ScenePack *)opts->dataClass;
	char fname[FILENAME_MAX+1];
	sprintf(fname,"%s.mesh",opts->path);

	int num_skeletons = 0;

	if (scenepack->num_meshes > 0 || scenepack->num_materials >0) {
		FILE *fd = fopen(fname, "wb");

		printf("Num Meshes: %d\n", scenepack->num_meshes);


		uint32_t version = CHCMESH_VERSION;
		fwrite(&version, sizeof(uint32_t), 1, fd);
		fwrite(&scenepack->num_meshes, sizeof(uint32_t), 1, fd);

		COLBBox box = get_scenepack_bbox(scenepack);
		fwrite(&box.min, sizeof(float), 3, fd);
		fwrite(&box.max, sizeof(float), 3, fd);

		for (int i = 0; i < scenepack->num_meshes; i++) {
			if(scenepack->m_meshes[i]->getNumBoneIndexSets() > 0 || scenepack->m_meshes[i]->getNumBoneIndexSets() > 0) {
				num_skeletons++;
			}
			write_mesh(scenepack->m_meshes[i], fd);
		}
		fwrite(&scenepack->num_materials, sizeof(uint32_t), 1, fd);
		for (int i = 0; i < scenepack->num_materials; i++) {
			write_material(scenepack->m_materials[i], fd);
		}

		fclose(fd);

	}


	//run textures through CHC tex exporter
	CTextureCollection *texture_collection = new CTextureCollection();
	for(int i=0;i<scenepack->num_materials;i++) {
		//materials[i]->getTexture(0)
		for(int j=0;j<MAX_MATERIAL_TEXTURES;j++) {
			CTexture *tex = scenepack->m_materials[i]->getTexture(j);
			if(tex != NULL) {
				texture_collection->AddTexture(tex);
			}
		}		
	}

	if (texture_collection->getTextures().size() > 0) {
		ExportOptions texopts;
		memset(&texopts, 0, sizeof(texopts));
		texopts.args = opts->args;
		texopts.dataClass = (void *)texture_collection;
		sprintf(fname, "%s.tex", opts->path);
		texopts.path = fname;
		texopts.srcPath = opts->srcPath;
		chc_tex_export_img(&texopts);
	}


	if(scenepack->m_collision) {
		sprintf(fname,"%s.col",opts->path);
		FILE *colfd = fopen(fname, "wb");
		write_collision(colfd, scenepack->m_collision);
		fclose(colfd);
	}

	if(num_skeletons > 0) {
		sprintf(fname,"%s.ske",opts->path);
		FILE *skefd = fopen(fname, "wb");
		uint32_t version = CHC_SKELETON_VERSION;
		fwrite(&version, sizeof(uint32_t), 1, skefd); //vesion
		version = 0;
		fwrite(&version, sizeof(uint32_t), 1, skefd); //flags
		fwrite(&scenepack->num_meshes, sizeof(uint32_t), 1, skefd);
		for(int i=0;i<scenepack->num_meshes;i++) {
			write_skeleton(scenepack->m_meshes[i], skefd);
		}
		
		fclose(skefd);
	}

	return false;
}