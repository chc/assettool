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