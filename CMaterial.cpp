#include "CMaterial.h"

CMaterial::CMaterial() {
	for(int i=0;i<4;i++) {
		m_specular_colour[i] = 0.0;
		m_ambient_colour[i] = 0.0;
		m_diffuse_colour[i] = 0.0;
	}

	m_shine = 0.0;
	m_shine_strength = 0.0;

	memset(&m_name,0,sizeof(m_name));

	memset(&m_textures,0,sizeof(m_textures));

	m_flags = 0;
}
CMaterial::~CMaterial() {
}
void CMaterial::setSpecColour(float r, float g, float b, float a) {
	m_specular_colour[0] = r;
	m_specular_colour[1] = g;
	m_specular_colour[2] = b;
	m_specular_colour[3] = a;

	m_flags |= EMaterialFlag_HasSpecColour;
}
void CMaterial::setAmbientColour(float r, float g, float b, float a) {
	m_ambient_colour[0] = r;
	m_ambient_colour[1] = g;
	m_ambient_colour[2] = b;
	m_ambient_colour[3] = a;

	m_flags |= EMaterialFlag_HasAmbientColour;
}
void CMaterial::setDiffuseColour(float r, float g, float b, float a) {
	m_diffuse_colour[0] = r;
	m_diffuse_colour[1] = g;
	m_diffuse_colour[2] = b;
	m_diffuse_colour[3] = a;

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
void CMaterial::getSpecColour(float &r, float &g, float &b, float &a) {
	r = m_specular_colour[0];
	g = m_specular_colour[1];
	b = m_specular_colour[2];
	a = m_specular_colour[3];
}
void CMaterial::getAmbientColour(float &r, float &g, float &b, float &a) {
	r = m_ambient_colour[0];
	g = m_ambient_colour[1];
	b = m_ambient_colour[2];
	a = m_ambient_colour[3];
}
void CMaterial::getDiffuseColour(float &r, float &g, float &b, float &a) {
	r = m_diffuse_colour[0];
	g = m_diffuse_colour[1];
	b = m_diffuse_colour[2];
	a = m_diffuse_colour[3];
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