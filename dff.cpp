#include "main.h"
#include "CGame.h"
#include "dff.h"
bool gta_rw_import_dff(ImportOptions* opts) {
	DFFHeader head;
	FILE *fd = fopen(opts->path,"rb");
	while(true) {
		fread(&head,sizeof(head),1,fd);
		fseek(fd,head.size,SEEK_CUR);
	}	
	fclose(fd);
	return true;
}
bool gta_rw_export_dff(ExportOptions *expOpts) {
	return false;
}