#ifndef _CDATARESOLVER_H
#define _CDATARESOLVER_H
class CDataResolver {
public:
	CDataResolver();
	virtual void resolve(sGenericData *input, sGenericData *output) = 0;
};
#endif //_CDATARESOLVER_H