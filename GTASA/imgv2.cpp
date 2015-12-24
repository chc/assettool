#include <main.h>
#include <Generic/CGame.h>
#include <Generic/CFile.h>
#include <GTASA/imgv2.h>
bool gta_rw_import_imgv2(ImportOptions* opts) {
	IMGV2Head head;
	IMGV2File *file_entries = NULL;
	FILE *fd = fopen(opts->path,"rb");
	fread(&head,sizeof(head),1,fd);
	file_entries = (IMGV2File *)malloc(sizeof(IMGV2File) * head.filecount);
	fread(file_entries,sizeof(IMGV2File),head.filecount,fd);
	CFile file;
	char *data;
	int sz;
	ExportOptions expOpts;
	for(int i=0;i<head.filecount;i++) {
		memset(&file,0,sizeof(file));
		memset(&expOpts,0,sizeof(expOpts));
		sz = file_entries[i].streamsize * 2048;
		data = (char *)malloc(sz);
		fseek(fd,file_entries[i].offset * 2048,SEEK_SET);
		fread(data,sz,1,fd);
		file.setData(data);
		file.setSize(sz);
		file.setName((char *)&file_entries[i].name);
		expOpts.dataClass = &file;
		expOpts.srcPath = opts->path;
		expOpts.path = opts->outpath;
		expOpts.args = opts->expArgs;
		opts->exporter(&expOpts);
		printf("Extracting: %s\n",file_entries[i].name);
		free(data);
	}
	fclose(fd);
	return true;
}
bool gta_rw_export_imgv2(ExportOptions *expOpts) {
	return false;
}