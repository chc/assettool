#ifndef _CGENERIC_H
#define _CGENERIC_H
#include <Generic/CGame.h>

enum ECoordinateSystem {
	//Vector coord systems
	ECoordinateSystem_Left,
	ECoordinateSystem_Left_XZY,
	ECoordinateSystem_Right,

	//Matrix coord systems
	ECoordinateSystem_MatrixLH_RM,
	ECoordinateSystem_MatrixRH_RM,
	ECoordinateSystem_MatrixLH_CM,
	ECoordinateSystem_MatrixRH_CM,
};


class CGeneric : public IGame {
public:
	CGeneric();
};
#endif //_CGTASA_H