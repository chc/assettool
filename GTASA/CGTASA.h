#ifndef _CGTASA_H
#define _CGTASA_H
#include <Generic/CGame.h>

#define GTA_FRAMES_PER_SECOND 60.0
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