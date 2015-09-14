#ifndef _GTA_RW_DFF
#define _GTA_RW_DFF
#include <stdint.h>
enum DFFTags {
	rwDATA = 1,
	rwSTRING = 2,
	rwEXTENSION = 3,
	rwTEXTURE = 6,
	rwMATERIALLIST = 8,
	rwMATERIAL = 7,
	rwFRAMELIST = 14,
	rwGEOMETRY = 15,
	rwCLUMP = 16,
	rwATOMIC = 20,
	rwGEOMETRYLIST = 26,
	rwANIMPLUGIN = 286,
	rwMATERIALEFFECTS = 288,
	rwMATERIALSPLIT = 1294,
	rwFRAME = 0x253F2FE,
	rwCOLORS = 0x253F2F9,
};
typedef struct _DFFHeader {
	DFFTags tag;
	uint32_t size;
	uint32_t version;
} DFFHeader;
bool gta_rw_export_dff(ExportOptions *expOpts);
bool gta_rw_import_dff(ImportOptions* opts);

#endif //_GTA_RW_DFF