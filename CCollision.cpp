#include <vector>
#include "CCollision.h"

CCollision::CCollision() {
}
void CCollision::addBBOX(BBox bbox) {
	m_bboxes.push_back(bbox);
}
std::vector<BBox> CCollision::getBBoxes() {
	return m_bboxes;
}