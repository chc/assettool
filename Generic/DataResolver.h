#ifndef _CDATARESOLVER_H
#define _CDATARESOLVER_H
#include <Generic/DataBank.h>
#include <Vector.h>
#include <Iterator.h>
class CDataResolver {
public:
	CDataResolver() { };
	virtual ~CDataResolver() {
		Core::Iterator<Core::Vector<sGenericData>, sGenericData> it = m_allocated_data.begin();
		while(it != m_allocated_data.end()) {
			sGenericData data = *it;
			if(data.type == EDataType_String_ASCII) { //we should change this thing to a class so we can just delete
				free(data.sUnion.mString);
			}
			it++;
		}
	}
	virtual sGenericData *resolve(sGenericData *input) = 0;
protected:
	Core::Vector<sGenericData> m_allocated_data;
};
#endif //_CDATARESOLVER_H