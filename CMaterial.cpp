#include "CMaterial.h"
/*
	float m_specular_colour[4];
	float m_ambient_colour[4];
	float m_diffuse_colour[4];

	float m_shine;
	float m_shine_strength;
	std::vector<CTexture *> m_textures;
*/
CMaterial::CMaterial() {
	for(int i=0;i<4;i++) {
		m_specular_colour[i] = 0.0;
		m_ambient_colour[i] = 0.0;
		m_diffuse_colour[i] = 0.0;
	}

	m_shine = 0.0;
	m_shine_strength = 0.0;
}
CMaterial::~CMaterial() {
}
void CMaterial::setSpecColour(float r, float g, float b, float a) {
	m_specular_colour[0] = r;
	m_specular_colour[1] = g;
	m_specular_colour[2] = b;
	m_specular_colour[3] = a;
}
void CMaterial::setAmbientColour(float r, float g, float b, float a) {
	m_ambient_colour[0] = r;
	m_ambient_colour[1] = g;
	m_ambient_colour[2] = b;
	m_ambient_colour[3] = a;
}
void CMaterial::setDiffuseColour(float r, float g, float b, float a) {
	m_diffuse_colour[0] = r;
	m_diffuse_colour[1] = g;
	m_diffuse_colour[2] = b;
	m_diffuse_colour[3] = a;
}
void CMaterial::setShine(float s) {
	m_shine = s;
}
void CMaterial::setShineStrength(float s) {
	m_shine_strength = s;
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