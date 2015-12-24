#ifndef _LIBTHPS_MISC_H
#define _LIBTHPS_MISC_H
#include "LibTHPS.h"
namespace LibTHPS {

bool	IsPowerOfTwo( int value );
void flip_8bit_buffer(uint8_t *colours, int width, int height);
void flip_32bit_buffer(uint32_t *colours, int width, int height, bool colour_fix = false);
void reverse_buffer(uint8_t *buff, uint8_t *out, int len);
void bgra_32bit_to_rgba(uint32_t *colours, int width, int height);
}
#endif //_LIBTHPS_MISC_H