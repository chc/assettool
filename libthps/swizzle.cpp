#include "swizzle.h"
#include <Windows.h>
#include <stdio.h>
static unsigned int	swizzle_table[4096];
static bool			swizzle_table_generated = false;

#define TWIDDLE(_u, _v) ((swizzle_table[(_v)] << 1) | (swizzle_table[(_u)]))

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
static void generateSwizzleTable( void )
{
	if( !swizzle_table_generated )
	{
		for( unsigned int i = 0, value = 0; i < 4096; i++ )
		{
			swizzle_table[i] = value;
			value += 0x2AAAAAAB;
			value &= 0x55555555;
		}
		swizzle_table_generated = true;
	}
}


/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void SwizzleTexture( void *dstBuffer, void *srcBuffer, int width, int height, int depth, int stride )
{
	int tilesX, tilesY;
    int tilesSizeX, tilesSizeY;
    int tileSize;

	// Tiny textures.
	if( width <= 2 || height <= 1 ) 
	{		
		memcpy( dstBuffer, srcBuffer, ( width * height * depth ) / 8 );
		return;
	} 

	// Okay, use the slow version.	
	generateSwizzleTable();

	if( width > height )
    {
        tilesX = width / height;
        tilesY = 1;

        tilesSizeX = width / tilesX;
        tilesSizeY = height;
    }
    else
    {
        tilesX = 1;
        tilesY = height / width;

        tilesSizeX = width;
        tilesSizeY = height / tilesY;
    }

    tileSize = tilesSizeX * tilesSizeY;

	switch (depth)
	{
		case 4:
	    case 8:
        {
			int j;

			for( j = 0; j < tilesY; j++)
            {
                int i;

                for (i = 0; i < tilesX; i++)
                {
					int y;
                    unsigned char *base;

                    base = (unsigned char *)(((unsigned char *)dstBuffer) +
                                       ((tileSize * tilesX) * j) +
                                       (tileSize * i));

                    for (y = 0; y < tilesSizeY; y++)
            {
                unsigned char    *srcPixel;
                int     x;

                        srcPixel = (unsigned char *)(((unsigned char *)srcBuffer) +
                                               (stride * (tilesSizeY * j)) +
                                               (tilesSizeX * i) +
                                               (stride * y));

                        for (x = 0; x < tilesSizeX; x++)
                {
                    unsigned char    *dstPixel;
                        dstPixel = (unsigned char *)(base + TWIDDLE(x, y));
		                *dstPixel = *srcPixel;

                    srcPixel++;
                }
            }
        }
            }
        }
        break;

    case 16:
        {
            int j;

            for (j = 0; j < tilesY; j++)
            {
                int i;

                for (i = 0; i < tilesX; i++)
                {
            int y;
                    unsigned char *base;

                    base = (unsigned char *)(((unsigned short *)dstBuffer) +
                                       ((tileSize * tilesX) * j) +
                                       (tileSize * i));

                    for (y = 0; y < tilesSizeY; y++)
            {
                unsigned short    *srcPixel;
                int     x;

                        srcPixel = (unsigned short *)(((unsigned char *)srcBuffer) +
                                                (stride * (tilesSizeY * j)) +
                                                (2 * tilesSizeX * i) +
                                                (stride * y));

                        for (x = 0; x < tilesSizeX; x++)
                {
                    unsigned short    *dstPixel;
                    dstPixel = (unsigned short *)(base + (TWIDDLE(x, y) << 1));
                    *dstPixel = *srcPixel;

                    srcPixel++;
                }
            }
        }
            }
        }
        break;

    case 24:
    case 32:
        {
            int j;

            for (j = 0; j < tilesY; j++)
            {
                int i;

                for (i = 0; i < tilesX; i++)
                {
            int y;
                    unsigned char *base;

                    base = (unsigned char *)(((unsigned int *)dstBuffer) +
                                       ((tileSize * tilesX) * j) +
                                       (tileSize * i));

                    for (y = 0; y < tilesSizeY; y++)
            {
                unsigned int    *srcPixel;
                int     x;

                        srcPixel = (unsigned int *)(((unsigned char *)srcBuffer) +
                                                (stride * (tilesSizeY * j)) +
                                                (4 * tilesSizeX * i) +
                                                (stride * y));

                        for (x = 0; x < tilesSizeX; x++)
                {
                    unsigned int    *dstPixel;
                    dstPixel = (unsigned int *)(base + (TWIDDLE(x, y) << 2));
                    *dstPixel = *srcPixel;

                    srcPixel++;
                }
            }
        }
            }
        }
        break;

    default:
		exit( 0 );
        break;
    }
}



/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void UnswizzleTexture( void *dstBuffer, void *srcBuffer,int width, int height, int depth, int stride )
{
	int tilesX, tilesY;
    int tilesSizeX, tilesSizeY;
    int tileSize;

	// Tiny textures.
	if( width <= 2 || height <= 1 ) 
	{		
		memcpy( srcBuffer, dstBuffer, ( width * height * depth ) / 8 );
		return;
	} 

	// Okay, use the slow version.	
	generateSwizzleTable();

	if( width > height )
    {
        tilesX = width / height;
        tilesY = 1;

        tilesSizeX = width / tilesX;
        tilesSizeY = height;
    }
    else
    {
        tilesX = 1;
        tilesY = height / width;

        tilesSizeX = width;
        tilesSizeY = height / tilesY;
    }

    tileSize = tilesSizeX * tilesSizeY;

	switch (depth)
	{
		case 4:
	    case 8:
        {
			int j;

			for( j = 0; j < tilesY; j++)
            {
                int i;

                for (i = 0; i < tilesX; i++)
                {
					int y;
                    unsigned char *base;

                    base = (unsigned char *)(((unsigned char *)srcBuffer) +
                                       ((tileSize * tilesX) * j) +
                                       (tileSize * i));

                    for (y = 0; y < tilesSizeY; y++)
            {
                unsigned char    *srcPixel;
                int     x;

                        srcPixel = (unsigned char *)(((unsigned char *)dstBuffer) +
                                               (stride * (tilesSizeY * j)) +
                                               (tilesSizeX * i) +
                                               (stride * y));

                        for (x = 0; x < tilesSizeX; x++)
                {
                    unsigned char    *dstPixel;
                        dstPixel = (unsigned char *)(base + TWIDDLE(x, y));
		                //*dstPixel = *srcPixel;
						*srcPixel = *dstPixel;

                    srcPixel++;
                }
            }
        }
            }
        }
        break;

    case 16:
        {
            int j;

            for (j = 0; j < tilesY; j++)
            {
                int i;

                for (i = 0; i < tilesX; i++)
                {
            int y;
                    unsigned char *base;

                    base = (unsigned char *)(((unsigned short *)srcBuffer) +
                                       ((tileSize * tilesX) * j) +
                                       (tileSize * i));

                    for (y = 0; y < tilesSizeY; y++)
            {
                unsigned short    *srcPixel;
                int     x;

                        srcPixel = (unsigned short *)(((unsigned char *)dstBuffer) +
                                                (stride * (tilesSizeY * j)) +
                                                (2 * tilesSizeX * i) +
                                                (stride * y));

                        for (x = 0; x < tilesSizeX; x++)
                {
                    unsigned short    *dstPixel;
                    dstPixel = (unsigned short *)(base + (TWIDDLE(x, y) << 1));
                    //*dstPixel = *srcPixel;
					*srcPixel = *dstPixel;

                    srcPixel++;
                }
            }
        }
            }
        }
        break;

    case 24:
    case 32:
        {
            int j;

            for (j = 0; j < tilesY; j++)
            {
                int i;

                for (i = 0; i < tilesX; i++)
                {
            int y;
                    unsigned char *base;

                    base = (unsigned char *)(((unsigned int *)srcBuffer) +
                                       ((tileSize * tilesX) * j) +
                                       (tileSize * i));

                    for (y = 0; y < tilesSizeY; y++)
            {
                unsigned int    *srcPixel;
                int     x;

                        srcPixel = (unsigned int *)(((unsigned char *)dstBuffer) +
                                                (stride * (tilesSizeY * j)) +
                                                (4 * tilesSizeX * i) +
                                                (stride * y));

                        for (x = 0; x < tilesSizeX; x++)
                {
                    unsigned int    *dstPixel;
                    dstPixel = (unsigned int *)(base + (TWIDDLE(x, y) << 2));
                    //*dstPixel = *srcPixel;
					*srcPixel = *dstPixel;

                    srcPixel++;
                }
            }
        }
            }
        }
        break;

    default:
		exit( 0 );
        break;
    }
}
