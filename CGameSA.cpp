#include "main.h"
#include "CGTASA.h"
#include "CImage.h"
#include "txd.h"
CGTASA::CGTASA() {
	FileFormat txdFmt;
	memset(&txdFmt,0,sizeof(txdFmt));
	txdFmt.description = "GTA: SA Texture Directory";
	txdFmt.name = "txd";
	txdFmt.mp_impFunc = gta_rw_import_txd;
	txdFmt.mp_expFunc = gta_rw_export_txd;
	m_fileFmtCount = 1;

	mp_shortName = "GTASA";
	mp_fullName = "Grand Theft Auto: San Andreas";
	mp_fileFormats = (FileFormat **)malloc(sizeof(FileFormat *) * m_fileFmtCount);
	mp_fileFormats[0] = (FileFormat *)malloc(sizeof(FileFormat));

	memcpy(mp_fileFormats[0],&txdFmt,sizeof(FileFormat));
}