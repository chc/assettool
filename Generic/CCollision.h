#ifndef _CCOLLISION_H
#define _CCOLLISION_H
#include <stdint.h>
#include <vector>
#include <Generic/CGeneric.h>
typedef struct {
	float min[3];
	float max[3];
	uint32_t checksum;
} COLBBox;

typedef struct {
	uint32_t *indices;
	float *verticies;
	uint32_t num_indices;
	uint32_t num_verts;
	uint32_t checksum;
} COLTriangleMesh;

typedef struct {
	float center[3];
	float radius;
} COLSphere;
/*
	More like collision collection
*/
class CCollision {
public:
	CCollision();
	void setBoundingBox(COLBBox box);
	void addBBOX(COLBBox bbox);
	void addTriMesh(COLTriangleMesh mesh);
	void addSphere(COLSphere sphere);

	COLBBox getBoundingBox();

	std::vector<COLBBox> getBBoxes();
	std::vector<COLTriangleMesh> getTriMeshes();
	std::vector<COLSphere> getSpheres();


	void setChecksum(uint32_t checksum); //the checksum for what it relates to
	uint32_t getChecksum();

	void addChild(CCollision *child);
	std::vector<CCollision *> getChildren();

	void setCoordinateSystem(ECoordinateSystem system) { m_coordinate_system = system; }
	ECoordinateSystem getCoordinateSystem(ECoordinateSystem handedness) { return m_coordinate_system; }
	void convertToCoordinateSystem(ECoordinateSystem system);
private:
	ECoordinateSystem m_coordinate_system;
	uint32_t m_checksum;
	COLBBox m_bbox;
	std::vector<COLSphere> m_spheres;
	std::vector<COLTriangleMesh> m_triangle_meshes;
	std::vector<COLBBox> m_bboxes;

	std::vector<CCollision *> m_children;
};
#endif //_CCOLLISION_H