#include "main.h"
#include "CGeneric.h"
#include "CImage.h"
#include "pngExporter.h"
CGeneric::CGeneric() {
	FileFormat txdFmt;
	memset(&txdFmt,0,sizeof(txdFmt));
	txdFmt.description = "PNG";
	txdFmt.name = "png";
	txdFmt.mp_impFunc = png_import_img;
	txdFmt.mp_expFunc = png_export_img;
	m_fileFmtCount = 1;

	mp_shortName = "Generic";
	mp_fullName = "Generic File Formats";
	mp_fileFormats = (FileFormat **)malloc(sizeof(FileFormat *) * m_fileFmtCount);
	mp_fileFormats[0] = (FileFormat *)malloc(sizeof(FileFormat));

	memcpy(mp_fileFormats[0],&txdFmt,sizeof(FileFormat));
}