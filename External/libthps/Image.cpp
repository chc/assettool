#include "Image.h"
#include "swizzle.h"
namespace LibTHPS {

Image::Image(EPlatform platform) { 
	m_platform = platform;
	m_valid = false;
	m_palette_data = NULL;

	m_colour_depth = v_UNKNOWN;
	m_palette_depth = v_UNKNOWN;
}
Image::Image(const char *path, EPlatform platform) {
	m_platform = platform;
	m_valid = false;
	m_palette_data = NULL;
	m_colour_data = NULL;

	m_colour_depth = v_UNKNOWN;
	m_palette_depth = v_UNKNOWN;

	loadFromFile(path);
}
Image::Image(void *mem, int len, EPlatform platform) {
	m_platform = platform;
	m_valid = false;
	m_palette_data = NULL;
	m_colour_data = NULL;

	m_colour_depth = v_UNKNOWN;
	m_palette_depth = v_UNKNOWN;
}

Image::~Image() {
	if(m_colour_data) {
		free(m_colour_data);
	}
	if(m_palette_data) {
		free(m_palette_data);
	}
}

void *Image::getRBGAData() {
	return NULL;
}

void Image::getDimensions(uint32_t &width, uint32_t &height) {
	width = m_width;
	height = m_height;
}
bool Image::loadFromFile(const char *path) {
	FILE *fd = fopen(path,"rb");

	if(!fd) return m_valid;

	uint32_t m_version;

	fread(&m_version,sizeof(uint32_t),1, fd);

	fread(&m_checksum,sizeof(uint32_t),1, fd);

	fread(&m_width,sizeof(uint32_t),1, fd);
	fread(&m_height,sizeof(uint32_t),1, fd);

	fread(&m_colour_depth,sizeof(uint32_t),1, fd);

	fread(&m_palette_depth,sizeof(uint32_t),1, fd);

	// Bits per texel and palette size.
	switch( m_colour_depth )
	{
		case PSMCT32:
			m_colour_depth = v32_BIT;
			break;
		case PSMCT16:
			m_colour_depth = v16_BIT;
			break;
		case PSMT8:
			m_colour_depth = v8_BIT;
			break;
	}
	int depth = 0;
	switch(m_colour_depth) {
		case v32_BIT:
			depth = 32;
			break;
		case v16_BIT:
			depth = 16;
			break;
		case v8_BIT:
			depth = 8;
		break;
	}
	// Bits per clut entry.
	if(	depth < 16 )
	{
		switch( m_palette_depth )
		{
			case PSMCT32:
				m_palette_depth = v32_BIT;
				break;
		}
	}
	else
	{
		m_palette_depth = v_UNKNOWN;
	}

	uint16_t m_original_width,m_original_height;

	fread(&m_original_width,sizeof(uint16_t),1,fd);
	fread(&m_original_height,sizeof(uint16_t),1,fd);

	uint32_t m_palette_size;

	fread(&m_palette_size,1,sizeof(uint32_t),fd);

	int palette_depth = 0;
	switch(m_palette_depth) {
		case v32_BIT:
			palette_depth = 32;
			break;
		case v16_BIT:
			palette_depth = 16;
			break;
		case v8_BIT:
			palette_depth = 8;
			break;
	}

	if(palette_depth != 0) {
		m_num_palette_entries = m_palette_size / (palette_depth/8);
		//if(m_num_palette_entries > 0) m_num_palette_entries++;

		//read palette data
		m_palette_data = (void *)malloc(m_palette_size);
		fread(m_palette_data, m_palette_size,1,fd);
	}

	uint32_t m_colour_size = m_width*m_height*(depth/8);

	m_colour_data = (void *)malloc(m_colour_size);
	fread(m_colour_data,m_colour_size,1,fd);

	switch(m_colour_depth) {
		case v32_BIT:
			//flip_32bit_buffer((uint32_t *)m_colour_data, m_width, m_height, true);
			break;
		case v16_BIT:
			break;
		case v8_BIT:
			//flip_8bit_buffer((uint8_t *)m_colour_data, m_width, m_height);
		break;
	}

	/*
		Unswizzle textue if xbox texture
	*/
	if(m_platform == Platform_Xbox) {
		if(IsPowerOfTwo(m_width) && IsPowerOfTwo(m_height)) {
			uint8_t *m_tmp_colour_data = (uint8_t*) malloc(m_colour_size);
			memcpy(m_tmp_colour_data, m_colour_data, m_colour_size);
			UnswizzleTexture( m_colour_data, m_tmp_colour_data,m_width, m_height, depth, m_width );
			//memcpy(m_colour_data, m_tmp_colour_data,m_colour_size);
			free(m_tmp_colour_data);
		}
	}

	m_valid = true;

	fclose(fd);

	return m_valid;
}

bool Image::isValid() {
	return m_valid;
}
}