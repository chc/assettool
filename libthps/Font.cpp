#include "LibTHPS.h"
#include "Font.h"
namespace LibTHPS {

Font::Font(EPlatform platform) {
	m_platform = platform;
}
Font::Font(const char *path, EPlatform platform) {
	m_platform = platform;
	m_image_data = NULL;
	loadFromFile(path);
}
Font::~Font() {
	if(m_image_data)
		free(m_image_data);

	if(m_character_data) {
		free(m_character_data);
	}
}

void Font::loadFromFile(const char *path) {
	FILE *fd = fopen(path,"rb");

	uint32_t file_size;

	fread(&file_size,sizeof(uint32_t),1,fd);

	fread(&m_num_chars,sizeof(uint32_t), 1, fd);
	fread(&m_default_height, sizeof(uint32_t), 1, fd);
	fread(&m_default_base, sizeof(uint32_t), 1, fd);

	int read_size = m_num_chars << 2;

	m_character_data = (SChar *)malloc(m_num_chars*sizeof(SChar));

	for(int i=0;i<m_num_chars;i++) {
		uint16_t baseline;
		int16_t ascii_val;
		fread(&baseline,sizeof(uint16_t),1,fd);
		fread(&ascii_val,sizeof(int16_t),1,fd);
		
		m_character_data[i].character = (uint8_t)ascii_val;
		m_character_data[i].Baseline = baseline;
	}

	uint32_t texture_size;
	uint16_t depth;

	fread(&texture_size,sizeof(uint32_t),1,fd);
	fread(&m_width,sizeof(uint16_t),1,fd);
	fread(&m_height,sizeof(uint16_t),1,fd);
	fread(&depth,sizeof(uint16_t),1,fd);

	fseek(fd,6,SEEK_CUR); //alignment

	uint32_t index_len = (m_width*m_height);
	m_image_data = (uint8_t*)malloc(index_len);

	fread(m_image_data,index_len,1,fd);

	fread(&m_palette,sizeof(m_palette),1,fd);


	for(int i=0;i<m_num_chars;i++) {
		fread(&m_character_data[i].x,sizeof(uint16_t),1,fd);
		fread(&m_character_data[i].y,sizeof(uint16_t),1,fd);
		fread(&m_character_data[i].w,sizeof(uint16_t),1,fd);
		fread(&m_character_data[i].h,sizeof(uint16_t),1,fd);
		m_character_data[i].u0 = (float)m_character_data[i].x / (float) m_character_data[i].h;
		m_character_data[i].v0 = (float)m_character_data[i].y / (float) m_character_data[i].h;
		m_character_data[i].u1 = m_character_data[i].u0 +  (float)m_character_data[i].x / (float) m_character_data[i].h;
		m_character_data[i].v1 = m_character_data[i].v0 +  (float)m_character_data[i].y / (float) m_character_data[i].w;

	}

	fclose(fd);
}

}