#ifndef _CKEYFRAMESEQCOLLECTION_H
#define _CKEYFRAMESEQCOLLECTION_H
#include <main.h>
#include "Vector.h"

class CKeyframeSequence;
class CKeyframeSeqCollection {
public:
	CKeyframeSeqCollection();
	~CKeyframeSeqCollection();

	void add(CKeyframeSequence *frame);
	Core::Vector<CKeyframeSequence *> getCollection();

	void setCollectionIdentifier(uint32_t checksum);
	uint32_t getCollectionIdentifier();

	void setBoneID(uint32_t bone_id);
	uint32_t getBoneID(uint32_t bone_id);
private:
	uint32_t bone_id;
	uint32_t checksum;
	Core::Vector<CKeyframeSequence *> frames;
};
#endif //_CKEYFRAMECOLLECTION_H
