#include "misc.h"

namespace LibTHPS {

bool	IsPowerOfTwo( int value )
{
	int num_bits_set;
	int mask;

	num_bits_set = 0;

	for( mask = 1; mask <= 0x8000000; mask <<= 1 )
	{
		if( mask & value )
		{
			num_bits_set++;
		}
	}
	
	return ( num_bits_set == 1 );
}
void reverse_buffer(uint8_t *buff, uint8_t *out, int len) {
	uint8_t *tmp = (uint8_t*)malloc(len);
	uint8_t *p = tmp;
	for(int i=len-1;i>=0;i--) {
		*p = buff[i];
		p++;
	}
	memcpy(out, tmp, len);
	free(tmp);
}
void reverse_buffer_32bit(uint32_t *buff, uint32_t *out, int len) {
	uint32_t *tmp = (uint32_t*)malloc(len*sizeof(uint32_t));
	uint32_t *p = tmp;
	for(int i=len-1;i>=0;i--) {
		*p = buff[i];
		p++;
	}
	memcpy(out, tmp, len*sizeof(uint32_t));
	free(tmp);
}
void flip_8bit_buffer(uint8_t *colours, int width, int height) {
	uint8_t *tmp = (uint8_t*)malloc(width*height*sizeof(uint8_t));
	reverse_buffer(colours, tmp, width*height*sizeof(uint8_t));
	memcpy(colours, tmp, width*height*sizeof(uint8_t));
	uint8_t *p = tmp;
	
	for(int i=0;i<height;i++) {
			reverse_buffer(&colours[i*width], p, width);
			p += width;
	}
	memcpy(colours, tmp, width*height*sizeof(uint8_t));
	free(tmp);
}
void flip_32bit_buffer(uint32_t *colours, int width, int height, bool colour_fix) {
	uint32_t *tmp = (uint32_t*)malloc((width*height)*sizeof(uint32_t));
	uint32_t *row = (uint32_t*)malloc((width)*sizeof(uint32_t));
	uint32_t *row_pixel = row;
	uint32_t *p = tmp;
	for (uint32_t y = height; y>0; y--) {
		for (uint32_t x = width; x>0; x--) {
			uint32_t b = ((colours[(y*width + x)]));
			uint32_t c = (b&(0xff << 24) | ((b&(0xff)) << 16)) | ((b&(0xff << 16))) >> 16 | (b & 0x0000ff00);
			if (!colour_fix) c = b;
			*(row_pixel++) = c;
		}
		row_pixel = row;
		reverse_buffer_32bit(row, p, width);
		//memcpy(p, row, width*sizeof(uint32_t));
		p += width;
	}
	memcpy(colours, tmp, width*height*sizeof(uint32_t));
	free(tmp);
}

void bgra_32bit_to_rgba(uint32_t *colours, int width, int height) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			uint32_t b = ((colours[(y*width + x)]));
			uint32_t c = (b&(0xff << 24) | ((b&(0xff)) << 16)) | ((b&(0xff << 16))) >> 16 | (b & 0x0000ff00);
			colours[(y*width + x)] = c;
		}
	}
}

}