#include <vector>
#include <Generic/CCollision.h>

CCollision::CCollision() {
	m_checksum = 0;
	memset(&m_bbox, 0, sizeof(m_bbox));
}
void CCollision::addBBOX(COLBBox bbox) {
	m_bboxes.push_back(bbox);
}
std::vector<COLBBox> CCollision::getBBoxes() {
	return m_bboxes;
}
void CCollision::addTriMesh(COLTriangleMesh mesh) {
	m_triangle_meshes.push_back(mesh);
}
void CCollision::addSphere(COLSphere sphere) {
	m_spheres.push_back(sphere);
}

std::vector<COLTriangleMesh> CCollision::getTriMeshes() {
	return m_triangle_meshes;
}
std::vector<COLSphere> CCollision::getSpheres() {
	return m_spheres;
}


void CCollision::setChecksum(uint32_t checksum) {
	m_checksum = checksum;
}
uint32_t CCollision::getChecksum() {
	return m_checksum;
}
void CCollision::setBoundingBox(COLBBox box) {
	m_bbox = box;
}
COLBBox CCollision::getBoundingBox() {
	return m_bbox;
}

void CCollision::addChild(CCollision *child) {
	m_children.push_back(child);
}
std::vector<CCollision *> CCollision::getChildren() {
	return m_children;
}