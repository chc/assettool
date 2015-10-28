#ifndef _CIMAGE_H
#define _CIMAGE_H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
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
	CTexture() {
		m_allocated = false;
		m_rgba = NULL;
		m_height = 0;
		m_width = 0;
	}
	~CTexture() {
		if(m_allocated) {
			if(m_rgba) {
				free(m_rgba);
			}
		}
	}
	void setDimensions(uint32_t width, uint32_t height) {
		m_width = width;
		m_height = height;
	}
	void setColourData(EColourType type, void *colour_data, int len = 0, int copy = 0) {
		m_colourType = type;
		if(copy) {
			m_rgba = malloc(len);
			memcpy(m_rgba,colour_data,len);
			m_allocated = true;
		} else {
			m_rgba = colour_data;
			m_allocated= false;
		}
		m_data_size = len;
	}
	void *getRGBA() {
		return m_rgba;
	}
	int getDataSize() {
		return m_data_size;
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
	bool m_allocated;
	uint32_t m_width;
	uint32_t m_height;
	EColourType m_colourType;
	void *m_rgba;
	int m_data_size; //sizeof m_rgba, useful for dxt
	void *m_palette;
};
#endif //_CIMAGE_H