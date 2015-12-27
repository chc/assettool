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
	EMaterialFlag_HasShineStrength = (1 << 6),
	EMaterialFlag_NoBackFaceCulling = (1 << 7),
	EMaterialFlag_NoCulling = (1 << 8),
	EMaterialFlag_HasTransparency = (1 << 9),
	EMaterialFlag_Opaque = (1 << 10),
};
enum EBlendMode
{
	EBlendMode_Diffuse,								// ( 0 - 0 ) * 0 + Src
	EBlendMode_Add,									// ( Src - 0 ) * Src + Dst
	EBlendMode_Fixed,								// ( Src - 0 ) * Fixed + Dst
	EBlendMode_Subtract,								// ( 0 - Src ) * Src + Dst
	EBlendMode_Subtract_Fixed,								// ( 0 - Src ) * Fixed + Dst
	EBlendMode_Blend,									// ( Src * Dst ) * Src + Dst	
	EBlendMode_Blend_Fixed,							// ( Src * Dst ) * Fixed + Dst	
	EBlendMode_Modulate,								// ( Dst - 0 ) * Src + 0
	EBlendMode_Modulate_Fixed,							// ( Dst - 0 ) * Fixed + 0	
	EBlendMode_Brighten,								// ( Dst - 0 ) * Src + Dst
	EBlendMode_Brighten_Fixed,							// ( Dst - 0 ) * Fixed + Dst	
	EBlendMode_GlossMap,								// Specular = Specular * Src	- special mode for gloss mapping
	EBlendMode_Previous_Mask,					// ( Src - Dst ) * Dst + Dst
	EBlendMode_Inverse_Previous_Mask,			// ( Dst - Src ) * Dst + Src
	EBlendMode_Modulate_Colour,	// ( Dst - 0 ) * Src(col) + 0	- special mode for the shadow.
	EBlendMode_OneInv_SrcAlpha,	//								- special mode for imposter rendering.

	vNUM_BLEND_MODES
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

	//used when the textures aren't loaded by converter
	void setTextureChecksum(uint32_t checksum, int level);
	uint32_t getTextureChecksum(int level);

	void setAmbientReflectionCoeff(float v);
	void setSpecularReflectionCoeff(float v);
	void setDiffuseReflectionCoeff(float v);

	float getAmbientReflectionCoeff();
	float getSpecularReflectionCoeff();
	float getDiffuseReflectionCoeff();
	EBlendMode getBlendMode(int level);
	void setBlendMode(EBlendMode mode, int level);
	uint64_t getFlags();
	void setFlag(uint64_t flags);
	void setIdentifierChecksum(uint32_t checksum); //internal game checksum, type_vehicle_main_body, etc
	uint32_t getIdentifierChecksum();

	static CMaterial *findMaterialByChecksum(CMaterial** mats, int num_mats, uint32_t checksum);
private:
	uint32_t m_identifier_checksum;
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
	uint32_t m_texture_checksums[MAX_MATERIAL_TEXTURES];

	EBlendMode m_texture_blend_modes[MAX_MATERIAL_TEXTURES];
};
#endif //_CMESH_H