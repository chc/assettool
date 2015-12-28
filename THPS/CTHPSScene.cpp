#include <THPS/CTHPSScene.h>
#include <Generic/CMesh.h>
#include <Generic/ScenePack.h>
int get_mesh_count(std::vector<LibTHPS::Sector *> secs) {
	int c = 0;
	std::vector<LibTHPS::Sector *>::iterator it = secs.begin();
	while(it != secs.end()) {

		LibTHPS::Sector *sec = *it;
		c += sec->getMeshes().size();	
		it++;
	}
	return c;
}
ETextureAddresingMode tex_address_mode_from_thps_uvaddress(uint32_t filter) {
	switch(filter) {
		default:
		case 0:
			return ETextureAddressMode_Wrap;
		case 1:
			return ETextureAddressMode_Clamp;
		case 2:
			return ETextureAddressMode_Border;
	}
}
void thps_material_to_cmaterial(LibTHPS::Material *mat, CMaterial *out) {
	out->setIdentifierChecksum(mat->getChecksum());
	int i = 0;
	while(i < 4) {
		LibTHPS::materialTexInfo matinfo = mat->getTexture(i);
		if(matinfo.m_texture_checksum == 0) break;
		out->setTextureChecksum(matinfo.m_texture_checksum, i);
		out->setBlendMode(EBlendMode_Modulate, i);
		out->setTextureAddressMode(tex_address_mode_from_thps_uvaddress(matinfo.m_u_address), tex_address_mode_from_thps_uvaddress(matinfo.m_v_address), i);
		i++;
	}
	
}
bool thps_xbx_import_scn(ImportOptions* opts) {
	LibTHPS::Scene *scn = new LibTHPS::Scene(opts->path, LibTHPS::Platform_Xbox);

	//load materials
	std::vector<LibTHPS::Material *> mats = scn->getMaterialList();
	std::vector<LibTHPS::Material *>::iterator it2 = mats.begin();
	CMaterial **out_mats = (CMaterial **)malloc(mats.size() * sizeof(CMaterial **));
	int i = 0;
	while(it2 != mats.end()) {
		LibTHPS::Material *mat = *it2;
		out_mats[i] = new CMaterial();
		thps_material_to_cmaterial(mat, out_mats[i]);
		i++;
		it2++;
	}

	std::vector<LibTHPS::Sector *> secs = scn->getSectorList();

	int mesh_count = get_mesh_count(secs);

	CMesh **out_meshes = (CMesh **)malloc(sizeof(CMesh *) * secs.size());
	
	std::vector<LibTHPS::Sector *>::iterator it = secs.begin();

	i = 0;

	//load sector stuff
	while(it != secs.end()) {

		LibTHPS::Sector *sec = *it;

		out_meshes[i] = new CMesh();
		out_meshes[i]->setNumVerts(sec->getNumVerticies());
		out_meshes[i]->setVerticies(sec->getPositions());
		out_meshes[i]->setNormals(sec->getNormals());
		out_meshes[i]->setGroupId(sec->getChecksum());
		out_meshes[i]->setColours(sec->getColours());

		float *uvws = (float *)malloc(sizeof(float) * 3 * sec->getNumVerticies());
		float *_uvs = sec->getTexCoords();
		float **uv_buffers = (float **)malloc(sizeof(float *) * sec->getNumTexChannels());
		float **uv_buffers_cursor = (float **)malloc(sizeof(float *) * sec->getNumTexChannels());
		float *p = _uvs;
		for(int j=0;j<sec->getNumTexChannels();j++) {
			uv_buffers[j] = (float *)malloc(sizeof(float)*3*sec->getNumVerticies());
			uv_buffers_cursor[j] = uv_buffers[j];
		}
		for(int j=0;j<sec->getNumVerticies();j++) {
			for(int k=0;k<sec->getNumTexChannels();k++) {
				memcpy(uv_buffers_cursor[k], p, sizeof(float)*2);
				uv_buffers_cursor[k][2] = 0.0;
				uv_buffers_cursor[k] += 3;
				p += 2;
			}
		}
	
		for(int k=0;k<sec->getNumTexChannels();k++) {
			out_meshes[i]->setUVWs(uv_buffers[k], k);
			free(uv_buffers[k]);
		}
		free(uv_buffers_cursor);
		free(uv_buffers);
		free(uvws);

		//load sector indices stuff
		std::vector<LibTHPS::Mesh *> meshes = sec->getMeshes();
		std::vector<LibTHPS::Mesh *>::iterator it2 = meshes.begin();

		out_meshes[i]->setIndexLevels(meshes.size());
		out_meshes[i]->setPrimType(CMeshPrimType_TriangleStrips);
		out_meshes[i]->setCoordinateSystem(ECoordinateSystem_Left_XZY);
		uint32_t j = 0;
		while(it2 != meshes.end()) {
			LibTHPS::Mesh *mesh = *it2;
			uint16_t *indices, *x;
			
			uint32_t num_indices = mesh->getCombinedIndices(&indices);
			x = indices;
			uint32_t *out_indices = (uint32_t *)malloc(num_indices * sizeof(uint32_t));
			uint32_t *p = out_indices;
			for(int c=0;c<num_indices;c++) {
				*p++ = *x++;
			}

			CMaterial *mat = CMaterial::findMaterialByChecksum(out_mats,mats.size() , mesh->getMaterialChecksum());
			out_meshes[i]->setIndexMaterial(mat, j);
			out_meshes[i]->setUseIndexedMaterials(true);
			out_meshes[i]->setIndices(out_indices, num_indices, j++);
			free(out_indices);
			free(indices);
			it2++;
		}

		i++;
		it++;
	}

	delete scn;

	ScenePack pack;
	memset(&pack, 0, sizeof(pack));
	pack.m_meshes = out_meshes;
	pack.num_meshes = secs.size();
	pack.m_materials = out_mats;
	pack.num_materials = mats.size();

	ExportOptions expopts;
	memset(&expopts,0,sizeof(expopts));
	
	expopts.dataClass = &pack;
	expopts.srcPath = opts->path;
	expopts.args = opts->expArgs;
	expopts.path = opts->outpath;
	opts->exporter(&expopts);
	return true;
}
bool thps_xbx_export_scn(ExportOptions* opts) {
	return false;
}