#include "CHCTexture.h"
#include <string.h>
#define CHC_TEX_VERSION 1
typedef struct {
	uint32_t version;
	uint32_t width;
	uint32_t height;
	EColourType colourType;
	uint32_t data_size;
} CHCTexHead;

bool chc_tex_import_img(ImportOptions *impOpts) {
	return false;
}
bool chc_tex_export_img(ExportOptions *expOpts) {
	FILE *fd = fopen(expOpts->path,"wb");
	CHCTexHead head;
	CTexture *tex = (CTexture *)expOpts->dataClass;
	memset(&head,0,sizeof(head));

	head.version = CHC_TEX_VERSION;
	head.colourType = tex->getColourType();
	head.data_size = tex->getDataSize();
	tex->getDimensions(head.width,head.height);
	fwrite(&head,sizeof(head),1,fd);
	fwrite(tex->getRGBA(),tex->getDataSize(),1,fd);
	fclose(fd);
	return false;
}