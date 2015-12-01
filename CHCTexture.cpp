#include "CHCTexture.h"
#include "ctexturecollection.h"
#include "Iterator.h"
#include "CTexture.h"
#include <string.h>
#define CHC_TEX_VERSION 2
typedef struct {
	uint32_t version;
	uint32_t num_images;
} CHCTexHead;

typedef struct {
	uint32_t checksum;
	uint32_t width;
	uint32_t height;
	EColourType type;
	uint32_t data_size;
} CHCTexItem;
bool chc_tex_import_img(ImportOptions *impOpts) {
	return false;
}

void chc_export_image_collection(FILE *fd, CTexture *tex) {
	CHCTexItem cube_item;
	CImage *img;
	memset(&cube_item, 0, sizeof(cube_item));
	if (tex->isCubeMap()) {
		cube_item.type = EColourType_CubeMap;
		cube_item.checksum = tex->getChecksum();
		fwrite(&cube_item, sizeof(cube_item), 1, fd);
		for (int i = 0; i < ECUBEMAPTYPE_COUNT; i++) {
			memset(&cube_item, 0, sizeof(cube_item));
			img = tex->getImage(i);
			img->compress();
			img->getDimensions(cube_item.width, cube_item.height);
			cube_item.data_size = img->getDataSize();
			cube_item.type = img->getColourType();
			cube_item.checksum = 0;
			fwrite(&cube_item, sizeof(cube_item), 1, fd);
			fwrite(img->getRawData(), cube_item.data_size, 1, fd);
		}
	}
	else {
		//write the raw img
		img = tex->getImage();
		img->compress();
		cube_item.type = img->getColourType();
		cube_item.data_size = img->getDataSize();
		cube_item.checksum = tex->getChecksum();
		img->getDimensions(cube_item.width, cube_item.height);
		fwrite(&cube_item, sizeof(cube_item), 1, fd);
		fwrite(img->getRawData(), cube_item.data_size, 1, fd);
	}
}
bool chc_tex_export_img(ExportOptions *expOpts) {
	FILE *fd = fopen(expOpts->path,"wb");
	CHCTexHead head;
	memset(&head,0,sizeof(head));
	CTextureCollection *tex = (CTextureCollection *)expOpts->dataClass;
	Core::Vector<CTexture *> textures = tex->getTextures();
	head.version = CHC_TEX_VERSION;
	head.num_images = textures.size();
	fwrite(&head,sizeof(head),1,fd);
	
	Core::Iterator<Core::Vector<CTexture *>, CTexture *> it = textures.begin();
	while (it != textures.end()) {
		chc_export_image_collection(fd, *it);
		it++;
	}
	fclose(fd);
	return false;
}