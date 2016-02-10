#ifndef _CGENERIC_H
#define _CGENERIC_H
#include <Generic/CGame.h>

enum ECoordinateSystem {
	ECoordinateSystem_Left,
	ECoordinateSystem_Left_XZY,
	ECoordinateSystem_Right,	
};

typedef struct {
	void *value;
	uint32_t identifier;
} DataMapEntry;

class CGeneric : public IGame {
public:
	CGeneric();
};
#endif //_CGTASA_H