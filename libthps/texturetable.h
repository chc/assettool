#ifndef _LIBTHPS_TEXTUETABLE_H
#define _LIBTHPS_TEXTURETABLE_H
#include "texture.h"
namespace LibTHPS {

class TextureTable {
public:
	TextureTable(const char *path, EPlatform platform);
	TextureTable(EPlatform platform);
	~TextureTable();
	Texture **getTextures(int &num_textures);
protected:
	uint32_t m_num_textures;
	Texture **m_textures;
	void loadFromFile(const char *path);
	EPlatform m_platform;
};

}
#endif //_LIBTHPS_TEXTURETABLE_H