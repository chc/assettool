/*

//extracting all txds in GTA
assettool -ingame GTASA -inpath gta3.img -infmt imgv2 -outpath gta3_out
assettool -ingame GTASA -inpath gta3_out -outpath png_out -infmt txd -outfmt png -inparams drop_mipmaps

//replace your imgs and rebuild to txd - in game not needed
assettool -infmt png -outfmt txd -inpath png_out -outpath newtxd.txd -outgame GTASA -outparams gen_mipmaps

//add TXD to gta3.img - outgame not needed here
assettool -ingame GTASA -infmt imgv2 -append newtxd.txd -outpath gta3_new.img
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#ifndef _MAIN_H
#define _MAIN_H
#include <Generic/CGame.h>
typedef struct  {
	IGame *gameIn;
	IGame *gameOut;

	const char *inPath;
	const char *outPath;
	FileFormat *inFmt;
	FileFormat *outFmt;

	const char *outFmtArgs;
	const char *inFmtArgs;
} assetToolState;

#define MAX_NAME 24
#ifndef _WIN32
	#define stricmp strcasecmp
	#define strnicmp strncasecmp
	#define sprintf_s snprintf
#endif
#endif //_MAIN_H