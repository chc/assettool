#ifndef _CKEYFRAMECOLLECTION_H
#define _CKEYFRAMECOLLECTION_H
#include <main.h>
#include "Vector.h"

class CKeyframeSeqCollection;
class CKeyframeCollection {
public:
	CKeyframeCollection();
	~CKeyframeCollection();

	void add(CKeyframeSeqCollection *col);
	Core::Vector<CKeyframeSeqCollection *> getCollection();

	void setCollectionIdentifier(uint32_t checksum);
	uint32_t getCollectionIdentifier();

	void setBoneID(uint32_t bone_id);
	uint32_t getBoneID(uint32_t bone_id);
private:
	uint32_t checksum;
	uint32_t bone_id;
	Core::Vector<CKeyframeSeqCollection *> frames;
};
#endif //_CKEYFRAMECOLLECTION_H
