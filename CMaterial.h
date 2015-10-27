#ifndef _CMATERIAL_H
#define _CMATERIAL_H
#include <stdint.h>
#include <vector>
#define MAX_MATERIAL_TEXTURES 4
class CTexture;
class CMaterial { 
public:
	CMaterial();
	~CMaterial();
	void setName(const char *name);
	void setSpecColour(float r, float g, float b, float a);
	void setAmbientColour(float r, float g, float b, float a);
	void setDiffuseColour(float r, float g, float b, float a);
	void setShine(float s);
	void setShineStrength(float s);


	float getShine();
	float getShineStrength();
	void getSpecColour(float &r, float &g, float &b, float &a);
	void getAmbientColour(float &r, float &g, float &b, float &a);
	void getDiffuseColour(float &r, float &g, float &b, float &a);
	const char *getName();

	void setTexture(CTexture *tex, int level);
	CTexture* getTexture(int level);
private:
	float m_specular_colour[4];
	float m_ambient_colour[4];
	float m_diffuse_colour[4];

	float m_shine;
	float m_shine_strength;

	char m_name[FILENAME_MAX+1];
	CTexture* m_textures[MAX_MATERIAL_TEXTURES];
};
#endif //_CMESH_H