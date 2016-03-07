#ifndef _CKEYFRAMESEQCOLLECTION_H
#define _CKEYFRAMESEQCOLLECTION_H
#include <main.h>
#include <Vector.h>
#include <Generic/DataBank.h>

class CKeyframeSequence;
class CKeyframeSeqCollection {
public:
	CKeyframeSeqCollection();
	~CKeyframeSeqCollection();

	void add(CKeyframeSequence *frame);
	Core::Vector<CKeyframeSequence *> getCollection();

	void setIdentifier(sGenericData identifier);
	sGenericData getIdentifier();

	void setBoneID(uint32_t bone_id);
	uint32_t getBoneID(uint32_t bone_id);
private:
	uint32_t bone_id;
	sGenericData m_identifier;
	Core::Vector<CKeyframeSequence *> frames;
};
#endif //_CKEYFRAMECOLLECTION_H
