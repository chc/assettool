#include "main.h"
#include "CGTASA.h"
#include "CImage.h"
#include "txd.h"
#include "imgv2.h"
#include "dff.h"
#include "ifp.h"
#include "col.h"
CGTASA::CGTASA() {
	FileFormat txdFmt;
	memset(&txdFmt,0,sizeof(txdFmt));
	txdFmt.description = "GTA: SA Texture Directory";
	txdFmt.name = "txd";
	txdFmt.mp_impFunc = gta_rw_import_txd;
	txdFmt.mp_expFunc = gta_rw_export_txd;
	m_fileFmtCount = 5;

	mp_shortName = "GTASA";
	mp_fullName = "Grand Theft Auto: San Andreas";
	mp_fileFormats = (FileFormat **)malloc(sizeof(FileFormat *) * m_fileFmtCount);

	mp_fileFormats[0] = (FileFormat *)malloc(sizeof(FileFormat));
	memcpy(mp_fileFormats[0],&txdFmt,sizeof(FileFormat));

	mp_fileFormats[1] = (FileFormat *)malloc(sizeof(FileFormat));
	memset(&txdFmt,0,sizeof(txdFmt));
	txdFmt.description = "GTA: IMG File Directory";
	txdFmt.name = "img";
	txdFmt.mp_impFunc = gta_rw_import_imgv2;
	txdFmt.mp_expFunc = gta_rw_export_imgv2;
	memcpy(mp_fileFormats[1],&txdFmt,sizeof(FileFormat));

	mp_fileFormats[2] = (FileFormat *)malloc(sizeof(FileFormat));
	memset(&txdFmt, 0, sizeof(txdFmt));
	txdFmt.description = "GTA: Object Data";
	txdFmt.name = "dff";
	txdFmt.mp_impFunc = gta_rw_import_dff;
	txdFmt.mp_expFunc = gta_rw_export_dff;
	memcpy(mp_fileFormats[2], &txdFmt, sizeof(FileFormat));

	mp_fileFormats[3] = (FileFormat *)malloc(sizeof(FileFormat));
	memset(&txdFmt, 0, sizeof(txdFmt));
	txdFmt.description = "GTA: Anim Data";
	txdFmt.name = "ifp";
	txdFmt.mp_impFunc = gta_rw_import_ifp;
	txdFmt.mp_expFunc = gta_rw_export_ifp;
	memcpy(mp_fileFormats[3], &txdFmt, sizeof(FileFormat));

	mp_fileFormats[4] = (FileFormat *)malloc(sizeof(FileFormat));
	memset(&txdFmt, 0, sizeof(txdFmt));
	txdFmt.description = "GTA: Collision Data";
	txdFmt.name = "col";
	txdFmt.mp_impFunc = gta_rw_import_col;
	txdFmt.mp_expFunc = gta_rw_export_col;
	memcpy(mp_fileFormats[4], &txdFmt, sizeof(FileFormat));
}