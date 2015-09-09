#include <png.h>
#include <stdlib.h>
#include <string.h>
#include <squish.h>
#include "CImage.h"
#include "pngExporter.h"
bool png_export_img(ExportOptions *expOpts) {
	CTexture *img = (CTexture *)expOpts->dataClass;
	FILE *fd = fopen(expOpts->path,"wb");
	if(!fd) return false;
	uint32_t width, height;
	img->getDimensions(width,height);

	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep row;
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	info_ptr = png_create_info_struct(png_ptr);	
	png_init_io(png_ptr, fd);
	png_byte color;
	int bit_depth = 0;
	EColourType colour = img->getColourType();
	int dxt_flags = 0, has_alpha = 0;
	void *rbga_data = img->getRGBA();
	switch(colour) {
		case EColourType_DXT1:
			dxt_flags = squish::kDxt1;
			break;
		case EColourType_DXT2:
			dxt_flags = squish::kDxt1;
			has_alpha = 1;
			break;
		case EColourType_DXT3:
			dxt_flags = squish::kDxt3;
			has_alpha = 1;
			break;
		case EColourType_DXT5:
			dxt_flags = squish::kDxt5;
			has_alpha = 1;
			break;

		case EColourType_32BPP:
			if(dxt_flags != 0) {
				char *cols_out = (char *)malloc(height*width*4);
				squish::DecompressImage((squish::u8*)cols_out,width,height,rbga_data,dxt_flags);
				colour = EColourType_32BPP;
				rbga_data = cols_out;
			}
			color = PNG_COLOR_TYPE_RGBA;
			bit_depth = 8;
			break;
		case EColourType_16BPP: 
			color = PNG_COLOR_TYPE_RGBA;
			bit_depth = 16;
			break;
		case EColourType_8BPP_256Palette:
			bit_depth = 8;
			color = PNG_COLOR_TYPE_PALETTE;
			png_color palette[256];
			png_byte transparency[256];
			memset(&palette,0,sizeof(palette));
			memset(&transparency,0,sizeof(transparency));
			for(int i=0;i<256;i++) {
				uint8_t *img_palette = (uint8_t*)img->getPalette();
				uint32_t col = img_palette[i];
				palette[i].red = (col) & 0xFF;			
				palette[i].green = (col >> 8) & 0xFF;
				palette[i].blue = (col >> 16) & 0xFF;
				transparency[i] = (col >> 24) & 0xFF;
			}
			png_set_PLTE(png_ptr,info_ptr,(png_colorp)&palette,256);
			png_set_tRNS(png_ptr, info_ptr, (png_bytep)&transparency,256, NULL);
			break;
	}


	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, width, height,
			8, color, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);
	row = (png_bytep) malloc(((sizeof(uint32_t) * width) * sizeof(png_byte)) + 4);

	if(colour == EColourType_32BPP) {
		for(uint32_t y1=0;y1<height;y1++) {
				for(uint32_t x1=0;x1<width;x1++) {
					uint32_t col = ((uint32_t *)rbga_data)[y1*width + x1];
					memcpy(&(row[x1 * 4]),&col, sizeof(uint32_t));
				}
				png_write_row(png_ptr, row);
		}
	} else if(colour == EColourType_16BPP) {
		//broken atm
		for(uint32_t y1=0;y1<height;y1++) {
				for(uint16_t x1=0;x1<width;x1++) {
					uint16_t col = ((uint16_t *)rbga_data)[y1*width + x1];
					memcpy(&(row[x1 * 2]),&col, sizeof(uint16_t));
				}
				png_write_row(png_ptr, row);
		}
	} else if(colour == EColourType_8BPP_256Palette) {
		for(uint32_t y1=0;y1<height;y1++) {
				for(uint32_t x1=0;x1<width;x1++) {
					uint8_t col = ((uint8_t *)rbga_data)[y1*width + x1];
					memcpy(&(row[x1]),&col, sizeof(uint8_t));
				}
				png_write_row(png_ptr, row);
		}
	}
	if(dxt_flags != 0) {
		//free the newly allocated colour data
		free(rbga_data);
	}
	// End write
	png_write_end(png_ptr, info_ptr);
	
	png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	
	png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	
	free(row);	
	fclose(fd);
	return true;
}
bool png_import_img(ImportOptions *impOpts) {
	FILE *fd = fopen(impOpts->path,"rb");
	if(!fd) return false;
	CTexture *tex = new CTexture();
	png_uint_32 width, height;
	int bit_depth, color_type;

	png_structp png_ptr;
	png_infop info_ptr;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,NULL);

	info_ptr = png_create_info_struct(png_ptr);	
	png_init_io(png_ptr, fd);
	png_byte color;
	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
		&color_type, NULL, NULL, NULL);

	/* tell libpng to strip 16 bit/color files down to 8 bits/color */
	png_set_strip_16(png_ptr);

	/* Strip alpha bytes from the input data without combining with the
	* background (not recommended).
	*/
	//png_set_strip_alpha(png_ptr);

	/* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
	* byte into separate bytes (useful for paletted and grayscale images).
	*/
	png_set_packing(png_ptr);

	/* Change the order of packed pixels to least significant bit first
	* (not useful if you are using png_set_packing). */
	//png_set_packswap(png_ptr);

	png_set_expand(png_ptr);

	/* Set the background color to draw transparent and alpha images over.
	* It is possible to set the red, green, and blue components directly
	* for paletted images instead of supplying a palette index.  Note that
	* even if the PNG file supplies a background, you are not required to
	* use it - you should use the (solid) application background if it has one.
	*/

	png_color_16 my_background, *image_background;

	if (png_get_bKGD(png_ptr, info_ptr, &image_background))
		png_set_background(png_ptr, image_background,
							PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);

	/* invert monochrome files to have 0 as white and 1 as black */
	png_set_invert_mono(png_ptr);

	/* If you want to shift the pixel values from the range [0,255] or
	* [0,65535] to the original [0,7] or [0,31], or whatever range the
	* colors were originally in:
	*/
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_sBIT))
	{
		png_color_8p sig_bit;

		png_get_sBIT(png_ptr, info_ptr, &sig_bit);
		png_set_shift(png_ptr, sig_bit);
	}

	/* swap the RGBA or GA data to ARGB or AG (or BGRA to ABGR) */
	//png_set_swap_alpha(png_ptr);

	/* swap bytes of 16 bit files to least significant byte first */
	png_set_swap(png_ptr);

	/* Add filler (or alpha) byte (before/after each RGB triplet) */
	//png_set_filler(png_ptr, 0x22, PNG_FILLER_AFTER);

	/* Turn on interlace handling.  REQUIRED if you are not using
	* png_read_image().  To see how to handle interlacing passes,
	* see the png_read_row() method below:
	*/
	int number_passes = png_set_interlace_handling(png_ptr);

	png_read_update_info(png_ptr,info_ptr);

	ExportOptions exp;
	exp.dataClass = (void *)tex;

	tex->setDimensions(width,height);

	int num_palette = 256, num_trans = 256;
	png_color palette[256];
	png_byte transparency[256];
	png_bytepp row_pointers;
	switch(color_type) {
	case PNG_COLOR_TYPE_PALETTE:
		png_get_PLTE(png_ptr,info_ptr,(png_colorp*)&palette,&num_palette);
		png_get_tRNS(png_ptr,info_ptr,(png_bytep*)&transparency,&num_trans,NULL);
	case PNG_COLOR_TYPE_RGB:
	case PNG_COLOR_TYPE_RGBA:
		switch(bit_depth) {
			case 8: {
				row_pointers = (png_bytepp)png_malloc(png_ptr, height * sizeof(png_bytep));

				for (int row = 0; row < height; row++)
				{
					row_pointers[row] = (png_bytep)png_malloc(png_ptr, png_get_rowbytes(png_ptr,info_ptr));
				}
				png_read_image(png_ptr,row_pointers);
				}
		}
		break;
	}

	//make this for all colour types
	void *col_data = malloc(sizeof(uint32_t) * width * height);
	char *p = (char *)col_data;
	int len = png_get_rowbytes(png_ptr,info_ptr);
	for(int i=0;i<height;i++) {
		memcpy(p,row_pointers[i],len);
		p += len;
	}

	tex->setColourData(EColourType_32BPP,col_data);

	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	/* close the file */
	fclose(fd);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	
	ExportOptions expOpts;
	memset(&expOpts,0,sizeof(expOpts));
	expOpts.path = impOpts->outpath;
	expOpts.dataClass = (void *)tex;
	expOpts.args = impOpts->expArgs;
	impOpts->exporter(&expOpts);
	return true;
}