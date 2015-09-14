#include "main.h"
#include "CGeneric.h"
#include "CImage.h"
#include "pngExporter.h"
#include "fileExport.h"
CGeneric::CGeneric() {
	FileFormat txdFmt;
	memset(&txdFmt,0,sizeof(txdFmt));
	txdFmt.description = "PNG";
	txdFmt.name = "png";
	txdFmt.mp_impFunc = png_import_img;
	txdFmt.mp_expFunc = png_export_img;
	m_fileFmtCount = 2;

	mp_shortName = "Generic";
	mp_fullName = "Generic File Formats";
	mp_fileFormats = (FileFormat **)malloc(sizeof(FileFormat *) * m_fileFmtCount);
	mp_fileFormats[0] = (FileFormat *)malloc(sizeof(FileFormat));
	mp_fileFormats[1] = (FileFormat *)malloc(sizeof(FileFormat));

	memcpy(mp_fileFormats[0],&txdFmt,sizeof(FileFormat));

	memset(&txdFmt,0,sizeof(txdFmt));

	txdFmt.description = "File System";
	txdFmt.name = "fs";
	txdFmt.mp_impFunc = gen_import_file;
	txdFmt.mp_expFunc = gen_export_file;

	memcpy(mp_fileFormats[1],&txdFmt,sizeof(FileFormat));

}