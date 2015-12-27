#include "CKeyframeSeqCollection.h"

CKeyframeSeqCollection::CKeyframeSeqCollection() {
	checksum = 0;
}
CKeyframeSeqCollection::~CKeyframeSeqCollection() {

}

void CKeyframeSeqCollection::add(CKeyframeSequence *frame) {
	frames.add(frame);
}
Core::Vector<CKeyframeSequence *> CKeyframeSeqCollection::getCollection() {
	return this->frames;
}

void CKeyframeSeqCollection::setCollectionIdentifier(uint32_t checksum) {
	this->checksum = checksum;
}
uint32_t CKeyframeSeqCollection::getCollectionIdentifier() {
	return checksum;
}

void CKeyframeSeqCollection::setBoneID(uint32_t bone_id) {
	this->bone_id = bone_id;
}
uint32_t CKeyframeSeqCollection::getBoneID(uint32_t bone_id) {
	return bone_id;
}