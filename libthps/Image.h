/*
	Code for reading THPS images and saving them
*/
#ifndef _LIBTHPS_IMG_H
#define _LIBTHPS_IMG_H

#include "LibTHPS.h"
#include "misc.h"

namespace LibTHPS {

enum ColourDepth {
	v32_BIT,
	v24_BIT,
	v16_BIT,
	v8_BIT,
	v4_BIT,		
	v8_BIT_GRAY,
	v4_BIT_GRAY,
	v2_BIT,
	v1_BIT,
	v2_BIT_GRAY,
	v1_BIT_GRAY,
	v_UNKNOWN,
};
#define IMG_MAX_MIPS 10 //could be all, but at least img

// Eeeek - the .img contains PS2 specific register values for bit depth.
// Use these values to convert them.
#define PSMCT32		0x00
#define PSMCT24		0x01
#define PSMCT16		0x02
#define PSMCT16S	0x0A
#define PS_GPU24	0x12
#define PSMT8		0x13
#define PSMT4		0x14
#define PSMT8H		0x1B
#define PSMT4HL		0x24
#define PSMT4HH		0x2C
#define PSMZ32		0x30
#define PSMZ24		0x31
#define PSMZ16		0x32
#define PSMZ16S		0x3A


class Image {
public:
	Image(EPlatform platform);
	Image(const char *path, EPlatform platform);
	Image(void *mem, int len, EPlatform platform);
	~Image();

	void *getRBGAData(); //convert palette, etc to RGBA

	bool isValid(); //successfully loaded image

	void getDimensions(uint32_t &width, uint32_t &height);

	ColourDepth getColourDepth() { return m_colour_depth; }
	ColourDepth getPaletteDepth() { return m_palette_depth; }
	int getNumPaletteEntries() { return m_num_palette_entries; }

	int getLevels() { return m_mip_levels; }
	uint32_t getChecksum() { return m_checksum; }

	uint32_t getDXT() { return m_dxt_e; }

	bool write(const char *path); //output to file in native platform format

	void *getPalette() { return m_palette_data; }
	void *getRawImgData() {
		return m_colour_data;
	}
protected:
	EPlatform m_platform;

	uint32_t m_checksum;
	uint32_t m_width;
	uint32_t m_height;
	
	ColourDepth m_colour_depth;
	ColourDepth m_palette_depth;

	uint32_t m_mip_levels;

	uint32_t m_dxt_e;

	void *m_colour_data; //array size = mip_levels

	int m_num_palette_entries;
	void *m_palette_data;

	bool m_valid;

	bool loadFromFile(const char *path);
};

}
#endif //_LIBTHPS_IMG_H