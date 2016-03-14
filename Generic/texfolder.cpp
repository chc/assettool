#include <FileSearcher.h>
#include <Generic/CGeneric.h>
#include <crc32.h>
#include <Generic/CTexture.h>
#include "pngExporter.h"
#include <Generic/CTextureCollection.h>
#include "texfolder.h"
bool gen_import_texfolder_process_image(ExportOptions *opts) {
	CImage *img = (CImage *)opts->dataClass;
	CTextureCollection *tex_col = (CTextureCollection *)opts->extra;
	CTexture *tex = new CTexture();

	const char *name = opts->srcPath;
	char chkstr[256];
	chkstr[0] = 0;
	char *last_slash = strrchr((char *)name, '/');
	if(last_slash) {
		name = last_slash+1;
	}

	char *last_period = strrchr((char *)name, '.');
	if(last_period) {
		*last_period = 0;
	}

	uint32_t checksum = crc32(0, name, strlen(name));
	printf("Checksum: %s : %08X\n",name, checksum);
	tex->setChecksum(checksum);
	tex->setImage(img);
	tex_col->AddTexture(tex);
	return true;

}
void texfolder_on_found_file(FileInfo *file,void *params) {
    ImportOptions *pngOpts = (ImportOptions *)params;
    printf("Searching: %s\n", file->name);
    pngOpts->path = file->name;
    png_import_img(pngOpts);

}
bool gen_tex_import_folder(ImportOptions* opts) {
	CTextureCollection *tex_col = new CTextureCollection();

	ImportOptions pngOpts;
	memset(&pngOpts, 0, sizeof(ImportOptions));
	pngOpts.exporter = gen_import_texfolder_process_image;
	pngOpts.outpath = opts->outpath;
	pngOpts.expArgs = opts->expArgs;
	pngOpts.path = opts->path;
	pngOpts.extra = (void *)tex_col;

	FileSearcher *searcher = new FileSearcher(opts->path, texfolder_on_found_file, &pngOpts);
	searcher->run();
	delete searcher;

	ExportOptions expOpts;
	memset(&expOpts, 0, sizeof(expOpts));
	expOpts.path = opts->outpath;
	expOpts.srcPath = opts->path;
	expOpts.dataClass = (void *)tex_col;
	expOpts.extra = opts->extra;
	expOpts.args = opts->expArgs;

	opts->exporter(&expOpts);

	return false;
}