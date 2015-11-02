#include "libthps\Image.h"
#include "CImage.h"
class CTHPSImg : public LibTHPS::Image {
public:
	CTHPSImg() : Image(LibTHPS::Platform_Xbox){ };
	bool write(CImage *img, const char *path);
};