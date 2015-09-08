#ifndef _LIBTHPS_FONT_H
#define _LIBTHPS_FONT_H
#include "LibTHPS.h"
#include <stdint.h>

namespace LibTHPS {

typedef struct
{
	uint16_t x,y;
	float	u0, v0, u1, v1;
	uint16_t	w, h;
	uint16_t	Baseline;
	uint8_t		character;
}
SChar;


class Font {
public:
	Font(EPlatform platform);
	Font(const char *path, EPlatform platform);
	Font(void *mem, int len, EPlatform platform);
	~Font();
	uint8_t *getImageData() { return m_image_data; }
	uint32_t *getPalette() {  return m_palette; }
	SChar *getCharacterData() { return m_character_data; }
	void getDimensions(uint32_t &width, uint32_t &height) {
		height = m_height;
		width = m_width;
	}
	uint32_t getNumCharacters() {
		return m_num_chars;
	}
private:
	void loadFromFile(const char *path);
	EPlatform m_platform;

	uint16_t m_width, m_height;

	uint32_t m_num_chars;
	uint32_t m_default_height;
	uint32_t m_default_base;

	SChar *m_character_data;

	uint8_t *m_image_data;
	uint32_t m_palette[256];
};

}
#endif //_LIBTHPS_FONT_H