#ifndef _CTHPSRESOLVER_H
#define _CTHPSRESOLVER_H
#include <Generic/DataResolver.h>
class CTHPSResolver : public CDataResolver {
public:
	CTHPSResolver();
	~CTHPSResolver();
	sGenericData *resolve(sGenericData *input);
};
#endif //_CTHPSRESOLVER_H