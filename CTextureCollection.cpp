#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "Map.h"
#include "CTextureCollection.h"
#include "CTexture.h"
CTextureCollection::CTextureCollection() {
}
CTextureCollection::~CTextureCollection() {
}
void CTextureCollection::AddTexture(CTexture *tex) {
	m_textures.add(tex);
}
Core::Vector<CTexture *> CTextureCollection::getTextures() {
	return m_textures;
}