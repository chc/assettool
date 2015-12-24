#include <main.h>
#include <Assimp/CAssImp.h>
#include <Assimp/AssImpModel.h>
CAssImp::CAssImp() {
	FileFormat txdFmt;
	memset(&txdFmt, 0, sizeof(txdFmt));
	txdFmt.description = "Model";
	txdFmt.name = "model";
	txdFmt.mp_impFunc = assimp_import_model;
	txdFmt.mp_expFunc = assimp_export_model;
	m_fileFmtCount = 1;

	mp_shortName = "assimp";
	mp_fullName = "Open Asset Importer";
	mp_fileFormats = (FileFormat **)malloc(sizeof(FileFormat *) * m_fileFmtCount);

	mp_fileFormats[0] = (FileFormat *)malloc(sizeof(FileFormat));
	memcpy(mp_fileFormats[0], &txdFmt, sizeof(FileFormat));
}