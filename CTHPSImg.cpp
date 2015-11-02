#include "CTHPSImg.h"
#include <libthps/misc.h>
bool CTHPSImg::write(CImage *img, const char *path) {
	FILE *fd = fopen(path,"wb");
	if(!fd) {
		return false;
	}
	uint32_t w,h;
	img->getDimensions(w,h);
	uint32_t version = 2, checksum = 0xCCCCCCCC, width = w, height = h;
	uint32_t colour_depth, palette_depth = 0, palette_size = 0, depth = 0;

	switch(img->getColourType()) {
	case EColourType_8BPP_256Palette:
		depth = 8;
		colour_depth = PSMT8;
		break;
	case EColourType_16BPP:
		depth = 16;
		colour_depth = PSMCT16;
		break;
	case EColourType_32BPP:
		depth = 32;
		colour_depth = PSMCT32;
		break;
	}
	fwrite(&version,sizeof(version),1,fd);
	fwrite(&checksum,sizeof(checksum),1,fd);
	fwrite(&width,sizeof(width),1,fd);
	fwrite(&height,sizeof(height),1,fd);

	fwrite(&colour_depth,sizeof(colour_depth),1,fd);
	fwrite(&palette_depth,sizeof(palette_depth),1,fd);

	uint16_t w16 = w,h16 = h;
	fwrite(&w16,sizeof(w16),1,fd);
	fwrite(&h16,sizeof(h16),1,fd);

	fwrite(&palette_size,sizeof(palette_size),1,fd);

	void *col_data = img->getRGBA();
	switch(colour_depth) {
		case PSMCT32:
			LibTHPS::flip_32bit_buffer((uint32_t *)col_data, width, height, true);
			break;
		case PSMCT16:
			break;
		case PSMT8:
			LibTHPS::flip_8bit_buffer((uint8_t *)col_data, width, height);
		break;
	}
	fwrite(img->getRGBA(),width*height*(depth/8),1,fd);


	fclose(fd);

	return true;
}