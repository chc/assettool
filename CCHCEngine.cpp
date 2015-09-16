#include "main.h"
#include "CCHCEngine.h"
#include "CHCMesh.h"
CCHCEngine::CCHCEngine() {
	FileFormat txdFmt;
	memset(&txdFmt,0,sizeof(txdFmt));
	txdFmt.description = "Generic Mesh";
	txdFmt.name = "mesh";
	txdFmt.mp_impFunc = chc_engine_import_mesh;
	txdFmt.mp_expFunc = chc_engine_export_mesh;
	m_fileFmtCount = 1;

	mp_shortName = "CHC";
	mp_fullName = "CHC Engine";
	mp_fileFormats = (FileFormat **)malloc(sizeof(FileFormat *) * m_fileFmtCount);

	mp_fileFormats[0] = (FileFormat *)malloc(sizeof(FileFormat));
	memcpy(mp_fileFormats[0],&txdFmt,sizeof(FileFormat));
}