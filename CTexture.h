#ifndef _CTEXTURE_H
#define _CTEXTURE_H
#include <stdint.h>
#include <stdio.h>
class CTexture { 
public:
	CTexture(const char *path);
	~CTexture();
	void setUVTiling(bool tile_u, bool tile_v);
	void setUVOffset(float u_offset, float v_offset);

	void getTile(bool &tile_u, bool &tile_v);
	void getOffset(float &u_offset, float &v_offset);
private:
	char path[FILENAME_MAX+1];
	bool tile_u;
	bool tile_v;
	float u_offset;
	float v_offset;
};
#endif //_CMESH_H