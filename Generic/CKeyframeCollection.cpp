#include "CKeyframeCollection.h"
#include <Generic/CGeneric.h>

CKeyframeCollection::CKeyframeCollection() {
}
CKeyframeCollection::~CKeyframeCollection() {

}

void CKeyframeCollection::add(CKeyframeSeqCollection *frame) {
	frames.add(frame);
}
Core::Vector<CKeyframeSeqCollection *> CKeyframeCollection::getCollection() {
	return this->frames;
}

void CKeyframeCollection::setIdentifier(sGenericData identifier) {
	m_identifier = identifier;
}
sGenericData CKeyframeCollection::getIdentifier() {
	return m_identifier;
}