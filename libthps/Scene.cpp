#include "LibTHPS.h"
#include "Scene.h"
namespace LibTHPS {

Scene::Scene(const char *path, EPlatform platform) {
	m_platform = platform;
	loadFromFile(path);
}

Scene::~Scene() {
}

void Scene::loadFromFile(const char *path) {
	uint32_t mat_version, mesh_version, vert_version;

	FILE *fd = fopen(path, "rb");

	fread(&mat_version,sizeof(uint32_t),1,fd);
	fread(&mesh_version,sizeof(uint32_t),1,fd);
	fread(&vert_version,sizeof(uint32_t),1,fd);

	// load mats
	m_material_list = Material::loadMaterialsFromScene(fd, m_platform);

	fread(&m_num_sectors,sizeof(uint32_t),1,fd);

	m_sectors = Sector::loadSectorsFromScene(fd, m_platform, m_num_sectors);

	fclose(fd);

	
}

std::vector<Material *> Scene::getMaterialList() {
	return m_material_list;
}
std::vector<Sector *> Scene::getSectorList() {
	return m_sectors;
}

}