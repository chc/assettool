#ifndef _CIMAGE_H
#define _CIMAGE_H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <squish.h>
#include "Utils.h"
enum EColourType {
	EColourType_8BPP_256Palette,
	EColourType_16BPP,
	EColourType_24BPP,
	EColourType_32BPP,
	EColourType_DXT1,
	EColourType_DXT2,
	EColourType_DXT3,
	EColourType_DXT5,
	EColourType_CubeMap, //not really a colour type, instead the images will follow with their types
};
class CImage {
public:
	CImage() {
		m_allocated = false;
		m_rgba = NULL;
		m_height = 0;
		m_width = 0;
	}
	~CImage() {
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
	void *getRawData() {
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
	/*
		Its assumed data is 32bit if this is happening
	*/
	int GetDXTLevel()
	{
		if(!isPowerOfTwo(m_width) || !isPowerOfTwo(m_height)) return 0;

		int has_holes = 0;
		uint32_t *pTex = (uint32_t *)getRGBA();
		for( int lp = 0; lp < ( m_width * m_height ); ++lp )
		{
			unsigned int alpha = (( pTex[lp] >> 24 ) & 0xFF );
			if(( alpha > 0 ) && ( alpha < 255 ))
			{
				// Texture has meaningful alpha. Requires DXT 5.
				return 5;
			}
			else if( alpha == 0 )
			{
				has_holes = 1;
			}
		}

		// If a texture has holes, requires DXT1A.
		if( has_holes )
			return 2;
	
		// Standard DXT.
		return 1;
	}

	void compress() {
		int level = GetDXTLevel();
		if(level == 0 || (m_colourType == EColourType_DXT1 || m_colourType == EColourType_DXT3 || m_colourType == EColourType_DXT5)) return;
		EColourType type;
		int flags = 0;
		switch(level) {
		case 2:
		case 1:
			flags |= squish::kDxt1;
			type = EColourType_DXT1;
			break;

		case 3:
			flags |= squish::kDxt3;
			type = EColourType_DXT3;
			break;
		case 5:
			flags |= squish::kDxt5;
			type = EColourType_DXT5;
			break;
		}

		int alloc_size = squish::GetStorageRequirements(m_width,m_height,flags);
		void *m_out_data = (void *)malloc(alloc_size);
		squish::CompressImage((squish::u8*)m_rgba,m_width,m_height,m_out_data,flags);

		m_colourType = type;
		free(m_rgba);
		m_rgba = m_out_data;
		m_data_size = alloc_size;
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