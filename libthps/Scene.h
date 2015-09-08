#ifndef _LIBTHPS_SCENE_H
#define _LIBTHPS_SCENE_H
#include "LibTHPS.h"
#include "Material.h"
#include "Sector.h"
#include <vector>
namespace LibTHPS {
class Scene {
public:
	Scene(const char *path, EPlatform platform);
	std::vector<Material *> getMaterialList();
	std::vector<Sector *> getSectorList();
	~Scene();
private:
	void loadFromFile(const char *path);

	EPlatform m_platform;
	uint32_t m_num_sectors;

	std::vector<Material *> m_material_list;
	std::vector<Sector *> m_sectors;
};
}
#endif // _LIBTHPS_SCENE_H