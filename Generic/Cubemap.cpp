#include <crc32.h>
#include <FileSearcher.h>
#include <Generic/CGeneric.h>
#include "Cubemap.h"
#include <Generic/CTexture.h>
#include "pngExporter.h"
#include <Generic/CTextureCollection.h>

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
		if (strstr(name, cubemap_types[i].prefix) != NULL) {
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
void cubemap_on_found_file(FileInfo *file,void *params) {
    ImportOptions *pngOpts = (ImportOptions *)params;
    printf("Searching: %s\n", file->name);
    pngOpts->path = file->name;
    CubemapParams *cparams = (CubemapParams *)pngOpts->extra;
	cparams->current_cube_type = findTypeByNamePrefix(file->name);
    png_import_img(pngOpts);

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
	if(opts->path[strlen(opts->path)] != '/')
		strcat(search_path, "//");

	CubemapParams params;
	params.tex = tex;
	pngOpts.extra = (void *)&params;



	FileSearcher *searcher = new FileSearcher(opts->path, cubemap_on_found_file, &pngOpts);
	searcher->run();
	delete searcher;

	uint32_t checksum = crc32(0, opts->path, strlen(opts->path));
	printf("%08X path\n", checksum);
	tex->setChecksum(checksum);
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
