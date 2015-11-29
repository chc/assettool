#include "CKeyframeCollection.h"

CKeyframeCollection::CKeyframeCollection() {
	checksum = 0;
}
CKeyframeCollection::~CKeyframeCollection() {

}

void CKeyframeCollection::add(CKeyframeSequence *frame) {
	frames.add(frame);
}
Core::Vector<CKeyframeSequence *> CKeyframeCollection::getCollection() {
	return this->frames;
}

void CKeyframeCollection::setCollectionIdentifier(uint32_t checksum) {
	this->checksum = checksum;
}
uint32_t CKeyframeCollection::getCollectionIdentifier() {
	return checksum;
}