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
private:
	float m_specular_colour[4];
	float m_ambient_colour[4];
	float m_diffuse_colour[4];

	float m_shine;
	float m_shine_strength;
	std::vector<CTexture *> m_textures;
};
#endif //_CMESH_H