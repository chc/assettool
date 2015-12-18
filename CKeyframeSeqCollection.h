#ifndef _CKEYFRAMESEQCOLLECTION_H
#define _CKEYFRAMESEQCOLLECTION_H
#include "main.h"
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
private:
	uint32_t checksum;
	Core::Vector<CKeyframeSequence *> frames;
};
#endif //_CKEYFRAMECOLLECTION_H
