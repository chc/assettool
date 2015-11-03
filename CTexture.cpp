#include "CImage.h"
#include "CTexture.h"
#include <string.h>

#include <squish.h>
CTexture::CTexture() {
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

void CTexture::setImage(CImage *img) {
	m_image = img;
}
CImage *CTexture::getImage() {
	return m_image;
}

uint32_t CTexture::getChecksum() {
	return m_checksum;
}
void CTexture::setChecksum(uint32_t checksum) {
	m_checksum = checksum;
}

void CTexture::compress() {
	m_image->compress();
}