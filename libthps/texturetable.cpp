#include "texturetable.h"
namespace LibTHPS {

TextureTable::TextureTable(const char *path, EPlatform platform) {
	m_platform = platform;
	loadFromFile(path);
}
TextureTable::~TextureTable() {

}
TextureTable::TextureTable(EPlatform platform) {
	m_platform = platform;
}
Texture **TextureTable::getTextures(int &num_textures) {
	num_textures = m_num_textures;
	return m_textures;
}
void TextureTable::loadFromFile(const char *path) {
	FILE *fd = fopen(path,"rb");

	uint32_t version;

	fread(&version,sizeof(uint32_t),1,fd);
	fread(&m_num_textures,sizeof(uint32_t),1,fd);

	Texture **textures = (Texture**)malloc(m_num_textures*sizeof(Texture *));

	for(uint32_t i=0;i<m_num_textures;i++) {
		textures[i] = new Texture(fd, m_platform);
	}

	m_textures = textures;

	

	fclose(fd);
}

}