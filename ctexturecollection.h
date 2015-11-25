#ifndef _GEN_TEXCOLLECTION_H
#define _GEN_TEXCOLLECTION_H
#include <stdio.h>
#include "Vector.h"
class CTexture;
class CTextureCollection {
public:
	CTextureCollection();
	~CTextureCollection();
	void AddTexture(CTexture *tex);
	Core::Vector<CTexture *> getTextures();
private:
	Core::Vector<CTexture *> m_textures;
};
#endif //_GEN_CUBEMAP_H