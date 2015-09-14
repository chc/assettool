#include "fileExport.h"
#include "CFile.h"
bool gen_import_file(ImportOptions *impOpts) {
	return false;
}
bool gen_export_file(ExportOptions *expOpts) {
	char path[FILENAME_MAX+1];
	CFile *file;
	file = (CFile *)expOpts->dataClass;
	sprintf(path,"%s/%s",expOpts->path,file->getName());
	FILE *fd = fopen(path,"wb");
	fwrite(file->getData(),file->getSize(),1,fd);
	fclose(fd);
	return false;
}