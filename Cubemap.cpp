#include "CGeneric.h"
#include "Cubemap.h"
#include "CTexture.h"
#include "pngExporter.h"
#include "ctexturecollection.h"
#include <Windows.h>
#include "crc32.h"
struct CubemapParams {
	CTexture *tex;
	uint8_t current_cube_type;
};

uint8_t findTypeByNamePrefix(const char *name) {
	struct CubemapTypeMap {
		const char *prefix;
		uint8_t type;
	} cubemap_types[] = {
		{ "nx", ECUBEMAPTYPE_NEGX },
		{ "ny", ECUBEMAPTYPE_NEGY },
		{ "nz", ECUBEMAPTYPE_NEGZ },
		{ "px", ECUBEMAPTYPE_POSX },
		{ "py", ECUBEMAPTYPE_POSY },
		{ "pz", ECUBEMAPTYPE_POSZ },
	};
	for (int i = 0; i < sizeof(cubemap_types) / sizeof(CubemapTypeMap); i++) {
		if (strncmp(name, cubemap_types[i].prefix, 2) == 0) {
			return cubemap_types[i].type;
		}
	}
	return -1;
}
bool gen_import_cubemap_process_image(ExportOptions *opts) {
	CImage *img = (CImage *)opts->dataClass;
	CubemapParams *params = (CubemapParams *)opts->extra;
	params->tex->setImage(img, params->current_cube_type);
	
	return true;

}
bool gen_import_cubemap(ImportOptions* opts) {
	CTexture *tex = new CTexture();
	ImportOptions pngOpts;
	memset(&pngOpts, 0, sizeof(ImportOptions));
	pngOpts.exporter = gen_import_cubemap_process_image;
	pngOpts.outpath = opts->outpath;
	pngOpts.expArgs = opts->expArgs;
	pngOpts.path = opts->path;
	char search_path[FILENAME_MAX];
	memset(&search_path, 0, sizeof(search_path));
	strcpy(search_path, opts->path);
	strcat(search_path, "//*");
	CubemapParams params;
	params.tex = tex;
	pngOpts.extra = (void *)&params;
	WIN32_FIND_DATA findData;
	HANDLE hndl = FindFirstFileA(search_path, &findData);
	do {
		if (~findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			sprintf(search_path, "%s/%s",opts->path, findData.cFileName);
			params.current_cube_type = findTypeByNamePrefix(findData.cFileName);
			pngOpts.path = search_path;
			png_import_img(&pngOpts);
		}
	} while (FindNextFile(hndl, &findData));

	FindClose(hndl);
	tex->setChecksum(crc32(0, opts->outpath, strlen(opts->outpath)));
	CTextureCollection *tex_col = new CTextureCollection();
	tex_col->AddTexture(tex);
	ExportOptions expOpts;
	memset(&expOpts, 0, sizeof(expOpts));
	expOpts.path = opts->outpath;
	expOpts.srcPath = opts->path;
	expOpts.dataClass = (void *)tex_col;
	expOpts.extra = opts->extra;
	expOpts.args = opts->expArgs;

	opts->exporter(&expOpts);
	//TODO: delete the textures inside :C
	delete tex;
	return true;
}
bool gen_export_cubemap(ExportOptions* opts) {
	return false;
}