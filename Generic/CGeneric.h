#ifndef _CGENERIC_H
#define _CGENERIC_H
#include <Generic/CGame.h>

enum ECoordinateHandedness {
	ECoordinateHandedness_Left,
	ECoordinateHandedness_Right,
};

class CGeneric : public IGame {
public:
	CGeneric();
};
#endif //_CGTASA_H