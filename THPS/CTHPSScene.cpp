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
void thps_material_to_cmaterial(LibTHPS::Material *mat, CMaterial *out) {
	out->setIdentifierChecksum(mat->getChecksum());
	int i = 0;
	while(i < 4) {
		LibTHPS::materialTexInfo matinfo = mat->getTexture(i);
		if(matinfo.m_texture_checksum == 0) break;
		out->setTextureChecksum(matinfo.m_texture_checksum, i);
		out->setBlendMode(EBlendMode_Modulate, i);
		i++;
	}
	
}
bool thps_xbx_import_scn(ImportOptions* opts) {
	LibTHPS::Scene *scn = new LibTHPS::Scene(opts->path, LibTHPS::Platform_Xbox);

	std::vector<LibTHPS::Sector *> secs = scn->getSectorList();

	int mesh_count = get_mesh_count(secs);

	CMesh **out_meshes = (CMesh **)malloc(sizeof(CMesh *) * secs.size());
	
	std::vector<LibTHPS::Sector *>::iterator it = secs.begin();

	int i = 0;

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
		for(int k=0;k<sec->getNumTexChannels();k++) {
			float *p = uvws;
			
			for(int j=0;j<sec->getNumVerticies();j++) {
				memcpy(p, _uvs, sizeof(float) * 2);
				p += 2;
				_uvs += 2;
				*p++ = 0.0;
			}
			out_meshes[i]->setUVWs(uvws, k);
		}
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
			uint32_t *out_indices = (uint32_t *)malloc(num_indices * 3 * sizeof(uint32_t));
			uint32_t *p = out_indices;
			for(int c=0;c<num_indices;c+=3) {
				*p++ = *x++;
				*p++ = *x++;
				*p++ = *x++;
			}
			out_meshes[i]->setIndices(out_indices, num_indices/3, j++);
			free(out_indices);
			free(indices);
			it2++;
		}

		i++;
		it++;
	}


	std::vector<LibTHPS::Material *> mats = scn->getMaterialList();
	std::vector<LibTHPS::Material *>::iterator it2 = mats.begin();
	CMaterial **out_mats = (CMaterial **)malloc(mats.size() * sizeof(CMaterial **));
	i = 0;
	while(it2 != mats.end()) {
		LibTHPS::Material *mat = *it2;
		out_mats[i] = new CMaterial();
		thps_material_to_cmaterial(mat, out_mats[i]);
		i++;
		it2++;
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