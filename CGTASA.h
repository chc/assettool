#ifndef _CGTASA_H
#define _CGTASA_H
#include "CGame.h"


enum EGTARWVersions {
	//san andreas versions
	ERWVersion_3_10 = 0xFFFF0310,
	ERWVersion_3_18 = 0xFFFF0318
};

class CGTASA : public IGame {
public:
	CGTASA();
};
#endif //_CGTASA_H