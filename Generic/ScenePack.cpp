#include <Generic/ScenePack.h>

COLBBox get_scenepack_bbox(ScenePack *pack) {
	float min[3], max[3];
	COLBBox box;
	box.checksum = 0;
	for (int i = 0; i < 3; i++) {
		min[i] = 9999999.0;
		max[i] = -9999999.0;
	}
	for (int i = 0; i < pack->num_meshes; i++) {
		float *verts = pack->m_meshes[i]->getVerticies();
		float *p = verts;
		int num_verts = pack->m_meshes[i]->getNumVertices();

		for (int j = 0; j < num_verts; j++) {
			for (int k = 0; k < 3; k++) {
				if (p[k] < min[k]) {
					min[k] = p[k];
				}
				if (p[k] > max[k]) {
					max[k] = p[k];
				}
			}
			p += 3;
		}
	}

	box.checksum = 1;
	for (int i = 0; i < 3; i++) {
		box.min[i] = min[i];
		box.max[i] = max[i];
	}
	return box;
}