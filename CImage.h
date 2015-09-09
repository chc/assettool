#ifndef _CIMAGE_H
#define _CIMAGE_H
#include <stdint.h>
enum EColourType {
	EColourType_8BPP_256Palette,
	EColourType_16BPP,
	EColourType_24BPP,
	EColourType_32BPP,
	EColourType_DXT1,
	EColourType_DXT2,
	EColourType_DXT3,
	EColourType_DXT5,
};
class CTexture {
public:
	void setDimensions(uint32_t width, uint32_t height) {
		m_width = width;
		m_height = height;
	}
	void setColourData(EColourType type, void *colour_data) {
		m_colourType = type;
		m_rgba = colour_data;
	}
	void *getRGBA() {
		return m_rgba;
	}
	EColourType getColourType() {
		return m_colourType;
	}
	void getDimensions(uint32_t &width, uint32_t &height) {
		width = m_width;
		height = m_height;
	}
	void setPalette(void *palette) {
		m_palette = palette;
	}
	void *getPalette() {
		return m_palette;
	}
protected:
	uint32_t m_width;
	uint32_t m_height;
	EColourType m_colourType;
	void *m_rgba;
	void *m_palette;
};
#endif //_CIMAGE_H