#include <Generic/CMaterial.h>
#include <crc32.h>
CMaterial::CMaterial() {
	for(int i=0;i<4;i++) {
		m_specular_colour[i][i] = 1.0;
		m_ambient_colour[i][i] = 1.0;
		m_diffuse_colour[i][i] = 1.0;
	}

	m_shine = 0.0;
	m_shine_strength = 0.0;
	m_identifier_checksum = 0;

	memset(&m_name,0,sizeof(m_name));

	memset(&m_textures,0,sizeof(m_textures));
	memset(&m_texture_checksums, 0, sizeof(m_texture_checksums));

	m_flags = 0;

	m_code = NULL;
	m_code_len = 0;
}
CMaterial::~CMaterial() {
}
void CMaterial::setSpecColour(float r, float g, float b, float a, int layer) {
	m_specular_colour[layer][0] = r;
	m_specular_colour[layer][1] = g;
	m_specular_colour[layer][2] = b;
	m_specular_colour[layer][3] = a;

	m_flags |= EMaterialFlag_HasSpecColour;
}
void CMaterial::setAmbientColour(float r, float g, float b, float a, int layer) {
	m_ambient_colour[layer][0] = r;
	m_ambient_colour[layer][1] = g;
	m_ambient_colour[layer][2] = b;
	m_ambient_colour[layer][3] = a;

	m_flags |= EMaterialFlag_HasAmbientColour;
}
void CMaterial::setDiffuseColour(float r, float g, float b, float a, int layer) {
	m_diffuse_colour[layer][0] = r;
	m_diffuse_colour[layer][1] = g;
	m_diffuse_colour[layer][2] = b;
	m_diffuse_colour[layer][3] = a;

	m_flags |= EMaterialFlag_HasDiffuseColour;
}
void CMaterial::setShine(float s) {
	m_shine = s;
}
void CMaterial::setShineStrength(float s) {
	m_shine_strength = s;
	m_flags |= EMaterialFlag_HasShineStrength;
}


float CMaterial::getShine() {
	return m_shine;
}
float CMaterial::getShineStrength() {
	return m_shine_strength;
}
void CMaterial::getSpecColour(float &r, float &g, float &b, float &a, int layer) {
	r = m_specular_colour[layer][0];
	g = m_specular_colour[layer][1];
	b = m_specular_colour[layer][2];
	a = m_specular_colour[layer][3];
}
void CMaterial::getAmbientColour(float &r, float &g, float &b, float &a, int layer) {
	r = m_ambient_colour[layer][0];
	g = m_ambient_colour[layer][1];
	b = m_ambient_colour[layer][2];
	a = m_ambient_colour[layer][3];
}
void CMaterial::getDiffuseColour(float &r, float &g, float &b, float &a, int layer) {
	r = m_diffuse_colour[layer][0];
	g = m_diffuse_colour[layer][1];
	b = m_diffuse_colour[layer][2];
	a = m_diffuse_colour[layer][3];
}

const char *CMaterial::getName() {
	return m_name;
}
void CMaterial::setName(const char *name) {
	strcpy(m_name, name);
}

void CMaterial::setTexture(CTexture *tex, int level) {
	m_textures[level] = tex;
}
CTexture* CMaterial::getTexture(int level) {
	return m_textures[level];
}
void CMaterial::setTextureChecksum(uint32_t checksum, int level) {
	m_texture_checksums[level] = checksum;
}
uint32_t CMaterial::getTextureChecksum(int level) {
	return m_texture_checksums[level];
}
void CMaterial::setTextureFilterMode(ETextureFilterMode mode, int level) {
	m_filter_modes[level] = mode;
	
}
void CMaterial::setTextureAddressMode(ETextureAddresingMode u, ETextureAddresingMode v, int level) {
	m_address_modes[level][0] = u;
	m_address_modes[level][1] = v;
}

void CMaterial::getTextureAddressModes(ETextureAddresingMode &u, ETextureAddresingMode &v, int level) {
	u = m_address_modes[level][0];
	v = m_address_modes[level][1];
}
ETextureFilterMode CMaterial::getTextureFilterMode(int level) {
	return m_filter_modes[level];
}

uint64_t CMaterial::getFlags() {
	return m_flags;
}
void CMaterial::setAmbientReflectionCoeff(float v) {
	m_ambient_intensity = v;
	m_flags |= EMaterialFlag_HasAmbientIntensitiy;
}
void CMaterial::setSpecularReflectionCoeff(float v) {
	m_specular_intensity = v;
	m_flags |= EMaterialFlag_HasSpecIntensitiy;
}
void CMaterial::setDiffuseReflectionCoeff(float v) {
	m_diffuse_intensity = v;
	m_flags |= EMaterialFlag_HasDiffuseIntensitiy;
}
float CMaterial::getAmbientReflectionCoeff() {
	return m_ambient_intensity;
}
float CMaterial::getSpecularReflectionCoeff() {
	return m_specular_intensity;
}
float CMaterial::getDiffuseReflectionCoeff() {
	return m_diffuse_intensity;
}
EBlendMode CMaterial::getBlendMode(int level) {
	return m_texture_blend_modes[level];
}
void CMaterial::setBlendMode(EBlendMode mode, int level) {
	m_texture_blend_modes[level] = mode;
}
void CMaterial::setIdentifierChecksum(uint32_t checksum) {
	m_identifier_checksum = checksum;
}
uint32_t CMaterial::getIdentifierChecksum() {
	if (m_identifier_checksum == 0) {
		m_identifier_checksum = crc32(0, m_name, strlen(m_name));
	}
	return m_identifier_checksum;
}
void CMaterial::setFlag(uint64_t flags) {
	m_flags |= flags;
}
CMaterial *CMaterial::findMaterialByChecksum(CMaterial** mats, int num_mats, uint32_t checksum) {
	for(int i=0;i<num_mats;i++) {
		if(mats[i]->getIdentifierChecksum() == checksum) {
			return mats[i];
		}
	}
	return NULL;
}
void CMaterial::setShaderCode(uint8_t *code, uint32_t len) {
	if(len == 0) return;
	m_code = (uint8_t *)malloc(len);
	m_code_len = len;
	memcpy(m_code, code, len);
	m_flags |= EMaterialFlag_HasShaderCode;
}
uint8_t *CMaterial::getShaderCode(uint32_t *len) {
	*len = m_code_len;
	return m_code;
}