#ifndef _CTEXTURE_H
#define _CTEXTURE_H
#include <stdint.h>
#include <stdio.h>
class CImage;
class CTexture { 
public:
	CTexture();
	~CTexture();
	void setUVTiling(bool tile_u, bool tile_v);
	void setUVOffset(float u_offset, float v_offset);

	void getTile(bool &tile_u, bool &tile_v);
	void getOffset(float &u_offset, float &v_offset);
	const char *getPath();

	void setImage(CImage *img);
	CImage *getImage();

	uint32_t getChecksum();
	void setChecksum(uint32_t checksum);
private:
	char path[FILENAME_MAX+1];
	bool tile_u;
	bool tile_v;
	float u_offset;
	float v_offset;

	CImage *m_image;

	uint32_t m_checksum;
};
#endif //_CMESH_H