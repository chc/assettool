#include "TTF.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <png.h>
inline int next_p2 (int a )
{
	int rval=1;
	// rval<<=1 Is A Prettier Way Of Writing rval*=2;
	while(rval<a) rval<<=1;
	return rval;
}
void saveFontTex(FT_GlyphSlot g) {
	FILE *fd = fopen("out.png","wb");
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep row;
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	info_ptr = png_create_info_struct(png_ptr);	
	png_init_io(png_ptr, fd);
	png_byte color = PNG_COLOR_TYPE_RGBA;

	int width = ( g->bitmap.width );
	int height =( g->bitmap.rows );

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, width, height,
			8, color, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	row = (png_bytep) malloc(((sizeof(uint32_t) * width) * sizeof(png_byte)) + 4);
	void *rbga_data = g->bitmap.buffer;
	for(uint32_t y1=0;y1<height;y1++) {			
		for(uint32_t x1=0;x1<width;x1++) {
				uint8_t col = ((uint8_t *)rbga_data)[y1*width + x1];
				uint32_t true_col = 0x00000000;
				true_col |= (col << 24);
				true_col |= (col << 16);
				true_col |= (col << 8);
				true_col |= (col);
				memcpy(&(row[x1 * 4]),&true_col, sizeof(uint32_t));
			}
			png_write_row(png_ptr, row);
	}
	// End write
	png_write_end(png_ptr, info_ptr);
	
	png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	
	png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	
	free(row);	
	fclose(fd);
}
void saveFont(char ch, FT_Library library, FT_Face face) {
	FT_GlyphSlot g = face->glyph;
	FT_Load_Char(face, ch, FT_LOAD_RENDER);
	saveFontTex(g);
}
bool ttf_import_mesh(ImportOptions* opts) {
	FT_Face face;
	FT_Library library;

	FT_Init_FreeType( &library );
	FT_New_Face( library, "test.ttf", 0, &face );
	//FT_Load_Glyph(
	int h = 16;
	FT_Set_Char_Size( face, h << 6, h << 6, 96, 96);
	FT_Set_Pixel_Sizes(face, 0, 48);

	saveFont('A',library,face);
	// We Don't Need The Face Information Now That The Display
	// Lists Have Been Created, So We Free The Assosiated Resources.
	FT_Done_Face(face);
 
	// Ditto For The Font Library.
	FT_Done_FreeType(library);
	return false;
}
bool ttf_export_mesh(ExportOptions* opts) {
	return false;
}