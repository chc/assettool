#ifndef _CFONT_H
#define _CFONT_H
#include <vector>
typedef struct {
	char ch;
	float u;
	float v;
} SChar;
class CFont {
public:
	void addChar(SChar character);
	void setImage(CImage *img);
};
#endif //_CFONT_H