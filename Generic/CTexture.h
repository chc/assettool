#ifndef _CTEXTURE_H
#define _CTEXTURE_H
#include <stdint.h>
#include <stdio.h>
class CImage;
enum CubemapTexTypes {
	ECUBEMAPTYPE_NEGX,
	ECUBEMAPTYPE_NEGY,
	ECUBEMAPTYPE_NEGZ,
	ECUBEMAPTYPE_POSX,
	ECUBEMAPTYPE_POSY,
	ECUBEMAPTYPE_POSZ,
	ECUBEMAPTYPE_COUNT,
	ECUBEMAPTYPE_NONE = -1,
};
class CTexture { 
public:
	CTexture();
	~CTexture();
	void setUVTiling(bool tile_u, bool tile_v);
	void setUVOffset(float u_offset, float v_offset);

	void getTile(bool &tile_u, bool &tile_v);
	void getOffset(float &u_offset, float &v_offset);

	void setPath(const char *path);
	const char *getPath();

	void setImage(CImage *img, uint8_t type = ECUBEMAPTYPE_NONE);
	CImage *getImage(uint8_t type = ECUBEMAPTYPE_NONE);

	uint32_t getChecksum();
	void setChecksum(uint32_t checksum);

	int GetDXTLevel();
	void compress();

	bool isCubeMap();
private:
	char path[FILENAME_MAX+1];
	bool tile_u;
	bool tile_v;
	float u_offset;
	float v_offset;

	CImage *m_image[ECUBEMAPTYPE_COUNT];
	bool is_cube_map;
	uint32_t m_checksum;
};
#endif //_CMESH_H