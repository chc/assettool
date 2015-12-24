#ifndef _LIBTHPS_TEXTURE_H
#define _LIBTHPS_TEXTURE_H
#include "Image.h"
#include <squish.h>
namespace LibTHPS {

enum EDxtOptions {
	eDXT1,
	eDXT2, //also known as dxt1a
	eDXT5,
};
class Texture : public Image {
public:
	Texture(FILE *fp, EPlatform platform);
	~Texture();
protected:
	EDxtOptions m_dxt;
};

}
#endif //_LIBTHPS_TEXTURE_H