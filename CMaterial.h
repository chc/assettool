#ifndef _CMATERIAL_H
#define _CMATERIAL_H
#include <stdint.h>
#include <vector>
#define MAX_MATERIAL_TEXTURES 4
class CTexture;
enum ETextureFilterMode {
	ETexFilterMode_None,
	ETexFilterMode_Nearest,
	ETexFilterMode_Linear,
	ETexFilterMode_MipNearest,
	ETexFilterMode_MipLinear,
	ETexFilterMode_LinearMipNearest,
	ETexFilterMode_LinearMipLinear,
};
enum ETextureAddresingMode {
	ETextureAddressMode_NoTiling,
	ETextureAddressMode_Wrap,
	ETextureAddressMode_Mirror,
	ETextureAddressMode_Clamp,
	ETextureAddressMode_Border,
};
enum EMaterialFlags {
	EMaterialFlag_HasAmbientIntensitiy = (1<<0),
	EMaterialFlag_HasDiffuseIntensitiy = (1<<1),
	EMaterialFlag_HasSpecIntensitiy = (1<<2),
	EMaterialFlag_HasAmbientColour = (1<<3),
	EMaterialFlag_HasDiffuseColour = (1<<4),
	EMaterialFlag_HasSpecColour = (1<<5),
	EMaterialFlag_HasShineStrength = (1<<6),
};
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
	
	void setTextureFilterMode(ETextureFilterMode mode, int level);
	void setTextureAddressMode(ETextureAddresingMode u, ETextureAddresingMode v, int level);

	void getTextureAddressModes(ETextureAddresingMode &u, ETextureAddresingMode &v, int level);
	ETextureFilterMode getTextureFilterMode(int level);

	void setTexture(CTexture *tex, int level);
	CTexture* getTexture(int level);

	void setAmbientReflectionCoeff(float v);
	void setSpecularReflectionCoeff(float v);
	void setDiffuseReflectionCoeff(float v);

	float getAmbientReflectionCoeff();
	float getSpecularReflectionCoeff();
	float getDiffuseReflectionCoeff();

	uint64_t getFlags();
private:
	uint64_t m_flags;

	float m_specular_colour[4];
	float m_ambient_colour[4];
	float m_diffuse_colour[4];

	float m_specular_intensity;
	float m_ambient_intensity;
	float m_diffuse_intensity;

	float m_shine;
	float m_shine_strength;

	char m_name[FILENAME_MAX+1];

	ETextureFilterMode m_filter_modes[MAX_MATERIAL_TEXTURES];
	ETextureAddresingMode m_address_modes[MAX_MATERIAL_TEXTURES][3]; //UVW
	CTexture* m_textures[MAX_MATERIAL_TEXTURES];
};
#endif //_CMESH_H