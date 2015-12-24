#include <main.h>
#include <THPS/CGameTHPS.h>
#include <Generic/CImage.h>
#include <Generic/CTexture.h>
#include <Generic/CTextureCollection.h>

#include <GTASA/txd.h>

#include <libthps/LibTHPS.h>
#include <libthps/Image.h>
#include <libthps/texturetable.h>
#include <libthps/misc.h>
#include "CTHPSImg.h"
using namespace LibTHPS;
CGameTHPS::CGameTHPS() {
	mp_shortName = "THPS";
	mp_fullName = "Tony Hawks Pro Skater Series";

	FileFormat fmt;
	memset(&fmt,0,sizeof(FileFormat));
	fmt.description = "THPS Image";
	fmt.name = "img";
	fmt.mp_impFunc = thps_xbx_import_img;
	fmt.mp_expFunc = thps_xbx_export_img;
	m_fileFmtCount = 2;


	mp_fileFormats = (FileFormat **)malloc(sizeof(FileFormat *) * m_fileFmtCount);
	mp_fileFormats[0] = (FileFormat *)malloc(sizeof(FileFormat));
	memcpy(mp_fileFormats[0],&fmt,sizeof(FileFormat));

	fmt.description = "THPS Texture Table";
	fmt.name = "tex";
	fmt.mp_impFunc = thps_xbx_import_textable;
	fmt.mp_expFunc = thps_xbx_export_textable;
	mp_fileFormats[1] = (FileFormat *)malloc(sizeof(FileFormat));
	memcpy(mp_fileFormats[1],&fmt,sizeof(FileFormat));
}

bool thps_xbx_import_img(ImportOptions* opts) {
	LibTHPS::Image *img = new LibTHPS::Image(opts->path,LibTHPS::Platform_Xbox);
	char out_path[FILENAME_MAX+1];
	ExportOptions expOpts;
	memset(&expOpts,0,sizeof(expOpts));
	expOpts.type = FileType_Texture;
	expOpts.path = (const char *)&out_path;
	CImage *tex = new CImage();
	uint32_t width, height;
	img->getDimensions(width,height);
	tex->setDimensions(width,height);
	EColourType colourType;

	switch(img->getColourDepth()) {
		case v32_BIT:
			colourType = EColourType_32BPP;
		break;
		case v16_BIT:
			colourType = EColourType_16BPP;
		break;
		case v8_BIT:
			colourType = EColourType_8BPP_256Palette;
		break;
	}
	tex->setColourData(colourType,(void *)img->getRawImgData());
	tex->setPalette(img->getPalette()); //will be NULL if not foudn but its all good!
	sprintf(out_path,"%s/%s.png",opts->outpath,opts->path);
	expOpts.dataClass = (void *)tex;
	opts->exporter(&expOpts);
	return false;
}
bool thps_xbx_export_img(ExportOptions *expOpts) {
	CTHPSImg img;
	memset(&img,0,sizeof(img));
	img.write((CImage *)expOpts->dataClass,expOpts->path);
	return false;
}
bool thps_xbx_export_textable(ExportOptions *expOpts) {
	return false;
}
EColourType get_colour_type_from_depth(LibTHPS::ColourDepth depth, uint32_t dxt) {
	if (dxt != 0) {
		switch (dxt) {
		case 1:
			return EColourType_DXT1;
		case 2:
			return EColourType_DXT2;
		case 3:
			return EColourType_DXT3;
		case 5:
			return EColourType_DXT5;
		}
	}
	switch (depth) {
	case 32:
		return EColourType_32BPP;
	case 16:
		return EColourType_16BPP;
	case 8: //for now assume its got a palette
		return EColourType_8BPP_256Palette;
	}
}
bool thps_xbx_import_textable(ImportOptions* opts) {
	char out_path[FILENAME_MAX+1];
	CTextureCollection *tex_col = new CTextureCollection();
	LibTHPS::TextureTable *textbl = new LibTHPS::TextureTable(opts->path, LibTHPS::Platform_Xbox);

	int count = 0;
	LibTHPS::Texture **texes = textbl->getTextures(count);

	
	ExportOptions expOpts;
	memset(&expOpts,0,sizeof(expOpts));
	expOpts.type = FileType_Texture;
	expOpts.dataClass = (void *)tex_col;

	expOpts.path = opts->outpath;
	expOpts.srcPath = opts->path;
	expOpts.extra = opts->extra;
	expOpts.args = opts->expArgs;

	uint32_t width,height;
	EColourType colourType = EColourType_32BPP;
	for(int i=0;i<count;i++) {
		texes[i]->getDimensions(width,height);

		CTexture *tex = new CTexture();
		CImage *img = new CImage();
		img->setDimensions(width, height);
		void *data = (void *)(((void **)texes[i]->getRawImgData())[0]);
		LibTHPS::ColourDepth depth = texes[i]->getColourDepth();
		EColourType coltype = get_colour_type_from_depth(depth, texes[i]->getDXT());
		img->setColourData(coltype, data);
		tex->setImage(img);
		tex->setChecksum(texes[i]->getChecksum());

		tex_col->AddTexture(tex);
	}

	opts->exporter(&expOpts);
	return false;
}
