#include <Vector.h>
#include "CKeyframeSeqCollection.h"

CKeyframeSeqCollection::CKeyframeSeqCollection() {
}
CKeyframeSeqCollection::~CKeyframeSeqCollection() {

}

void CKeyframeSeqCollection::add(CKeyframeSequence *frame) {
	frames.add(frame);
}
Core::Vector<CKeyframeSequence *> CKeyframeSeqCollection::getCollection() {
	return this->frames;
}

void CKeyframeSeqCollection::setIdentifier(sGenericData identifier) {
	m_identifier = identifier;
}
sGenericData CKeyframeSeqCollection::getIdentifier() {
	return m_identifier;
}

void CKeyframeSeqCollection::setBoneID(uint32_t bone_id) {
	this->bone_id = bone_id;
}
uint32_t CKeyframeSeqCollection::getBoneID(uint32_t bone_id) {
	return bone_id;
}