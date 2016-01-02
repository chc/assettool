#ifndef _MAIN_LIBTHPS
#define _MAIN_LIBTHPS

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
namespace LibTHPS {

enum EPlatform {
	Platform_Unknown, //can't process
	Platform_Xbox,
	Platform_PS2,
	Platform_NGC,
	Platform_Win32,
};

enum EGame {
	Game_THPS3,
	Game_THUG,
};

}
#endif // _MAIN_LIBTHPS