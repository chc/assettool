#ifndef _CCOLLISION_H
#define _CCOLLISION_H
#include <stdint.h>
typedef struct {
	float min[3];
	float max[3];
	uint32_t checksum;
} BBox;
/*
	More like collision collection
*/
class CCollision {
public:
	CCollision();
	void addBBOX(BBox bbox);
	std::vector<BBox> getBBoxes();
private:
	std::vector<BBox> m_bboxes;
};
#endif //_CCOLLISION_H