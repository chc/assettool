#ifndef _CKEYFRAMECOLLECTION_H
#define _CKEYFRAMECOLLECTION_H
#include "main.h"
#include "Vector.h"

class CKeyframeSequence;
class CKeyframeCollection {
public:
	CKeyframeCollection();
	~CKeyframeCollection();

	void add(CKeyframeSequence *frame);
	Core::Vector<CKeyframeSequence *> getCollection();

	void setCollectionIdentifier(uint32_t checksum);
	uint32_t getCollectionIdentifier();
private:
	uint32_t checksum;
	Core::Vector<CKeyframeSequence *> frames;
};
#endif //_CKEYFRAMECOLLECTION_H
