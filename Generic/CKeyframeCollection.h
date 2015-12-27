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

private:
	uint32_t checksum;
	Core::Vector<CKeyframeSeqCollection *> frames;
};
#endif //_CKEYFRAMECOLLECTION_H
