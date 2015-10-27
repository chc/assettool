#include "CTexture.h"
#include <string.h>
CTexture::CTexture(const char *path) {
	strcpy(this->path,path);
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