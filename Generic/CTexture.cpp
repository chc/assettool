#include <Generic/CImage.h>
#include <Generic/CTexture.h>
#include <string.h>

#include <squish.h>
CTexture::CTexture() {
	memset(&m_image, 0, sizeof(m_image));
	is_cube_map = false;
	path[0] = 0;
}
CTexture::~CTexture() {
}
void CTexture::setUVTiling(bool tile_u, bool tile_v) {
	this->tile_u = tile_u;
	this->tile_v = tile_v;

}
void CTexture::setUVOffset(float u_offset, float v_offset) {
	this->u_offset = u_offset;
	this->v_offset = v_offset;
}

void CTexture::getTile(bool &tile_u, bool &tile_v) {
	tile_u = this->tile_u;
	tile_v = this->tile_v;
}
void CTexture::getOffset(float &u_offset, float &v_offset) {
	u_offset = this->u_offset;
	v_offset = this->v_offset;
}
void CTexture::setPath(const char *path) {
	strcpy(this->path,path);
}
const char *CTexture::getPath() {
	return (const char *)&path;
}

void CTexture::setImage(CImage *img,uint8_t type) {
	if (type < 0 || type > ECUBEMAPTYPE_COUNT) type = 0;
	else is_cube_map = true;
	m_image[type] = img;
}
CImage *CTexture::getImage(uint8_t type) {
	if (type < 0 || type > ECUBEMAPTYPE_COUNT) type = 0;
	return m_image[type];
}

bool CTexture::isCubeMap() {
	return is_cube_map;
}
uint32_t CTexture::getChecksum() {
	return m_checksum;
}
void CTexture::setChecksum(uint32_t checksum) {
	m_checksum = checksum;
}

void CTexture::compress() {
	for (int i = 0; i < ECUBEMAPTYPE_COUNT; i++) {
		if(m_image[i] != NULL)
			m_image[i]->compress();
	}
	
}