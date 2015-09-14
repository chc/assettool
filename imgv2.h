#ifndef _GTA_IMGV2_H
#define _GTA_IMGV2_H
#include <stdint.h>
typedef struct _IMGV2Head {
	uint32_t magic;
	uint32_t filecount;
} IMGV2Head;

typedef struct _IMGV2File {
	uint32_t offset;
	uint16_t streamsize;;
	uint16_t size;
	char name[24];
} IMGV2File;

bool gta_rw_import_imgv2(ImportOptions* opts);
bool gta_rw_export_imgv2(ExportOptions *expOpts);
#endif //_GTA_IMGV2_H