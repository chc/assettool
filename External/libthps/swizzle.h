#ifndef _SWIZZLE_H
#define _SWIZZLE_H
#include <stdint.h>
void SwizzleTexture( void *dstBuffer, void *srcBuffer, int width, int height, int depth, int stride );
void UnswizzleTexture( void *dstBuffer, void *srcBuffer,int width, int height, int depth, int stride );
#endif