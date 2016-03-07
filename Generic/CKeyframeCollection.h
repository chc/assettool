#ifndef _CKEYFRAMECOLLECTION_H
#define _CKEYFRAMECOLLECTION_H
#include <main.h>
#include "Vector.h"
#include <Generic/DataBank.h>

class CKeyframeSeqCollection;
class CKeyframeCollection {
public:
	CKeyframeCollection();
	~CKeyframeCollection();

	void add(CKeyframeSeqCollection *col);
	Core::Vector<CKeyframeSeqCollection *> getCollection();

	void setIdentifier(sGenericData identifier);
	sGenericData getIdentifier();

private:
	sGenericData m_identifier;
	Core::Vector<CKeyframeSeqCollection *> frames;
};
#endif //_CKEYFRAMECOLLECTION_H
