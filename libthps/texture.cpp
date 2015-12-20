#include "texture.h"
#include "swizzle.h"
namespace LibTHPS {

Texture::Texture(FILE *fd, EPlatform platform) : Image(platform) {
	m_colour_data = NULL;
	m_palette_data = NULL;

	fread(&m_checksum,sizeof(uint32_t),1,fd);
	fread(&m_width,sizeof(uint32_t),1,fd);
	fread(&m_height,sizeof(uint32_t),1,fd);

	fread(&m_mip_levels,sizeof(uint32_t),1,fd);
	fread(&m_colour_depth,sizeof(uint32_t),1,fd);
	fread(&m_palette_depth,sizeof(uint32_t),1,fd);

	uint32_t dxt;
	fread(&dxt,sizeof(uint32_t),1,fd);

	m_dxt_e = dxt;

	uint32_t palette_size;
	fread(&palette_size,sizeof(uint32_t),1,fd);

	if(palette_size != 0) {
		m_palette_data = malloc(palette_size);

		m_num_palette_entries = palette_size / (m_palette_depth / 8);

		fread(m_palette_data,m_num_palette_entries,1,fd);
	}

	void **colours = (void **)malloc(m_mip_levels*sizeof(void *));
	

	//get all texel sizes to allocate for reading
	for(uint32_t i = 0;i<m_mip_levels;i++) {

		uint32_t texel_size;
		fread(&texel_size,sizeof(uint32_t),1,fd);
		colours[i] = (void *)malloc(texel_size);

		fread(colours[i], texel_size, 1, fd);
	}
	/*
	if(dxt != 0) {
		int dxt_flags = 0;
		char has_alpha = 0;
		switch(dxt) {
			case 1:
				dxt_flags = squish::kDxt1;
				break;
			case 2:
				dxt_flags = squish::kDxt1;
				has_alpha = 1;
				break;
			case 3:
				dxt_flags = squish::kDxt3;
				has_alpha = 1;
				break;
			case 5:
				dxt_flags = squish::kDxt5;
				has_alpha = 1;
				break;			
		}
		void **decompressed_colours = (void **)malloc(m_mip_levels*sizeof(void*));
		uint32_t w = m_width, h = m_height;
		for(uint32_t i = 0;i<m_mip_levels;i++) {


			decompressed_colours[i] = (void *)malloc(w*h*sizeof(uint32_t)+4000);
			squish::DecompressImage((squish::u8*)decompressed_colours[i], w, h, colours[i], dxt_flags);
			flip_32bit_buffer((uint32_t *)decompressed_colours[i], w, h);
			w /= 2;
			h /= 2;
			free(colours[i]);
		}
		free(colours);
		colours = decompressed_colours;
	}
	*/


	m_colour_data = (void*)colours;

	//free(colours);
}

Texture::~Texture() {
	free(m_colour_data);
	if(m_palette_data)
		free(m_palette_data);
}

}