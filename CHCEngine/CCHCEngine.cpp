#include <main.h>
#include <CHCEngine/CCHCEngine.h>
#include "CHCMesh.h"
#include <CHCEngine/CHCTexture.h>
#include "CCHCKeyframe.h"
CCHCEngine::CCHCEngine() {
	FileFormat txdFmt;
	memset(&txdFmt,0,sizeof(txdFmt));
	txdFmt.description = "Generic Mesh";
	txdFmt.name = "mesh";
	txdFmt.mp_impFunc = chc_engine_import_mesh;
	txdFmt.mp_expFunc = chc_engine_export_mesh;

	m_fileFmtCount = 4;

	mp_shortName = "CHC";
	mp_fullName = "CHC Engine";
	mp_fileFormats = (FileFormat **)malloc(sizeof(FileFormat *) * m_fileFmtCount);

	mp_fileFormats[0] = (FileFormat *)malloc(sizeof(FileFormat));
	memcpy(mp_fileFormats[0],&txdFmt,sizeof(FileFormat));

	memset(&txdFmt, 0, sizeof(txdFmt));
	txdFmt.description = "Texture";
	txdFmt.name = "tex";
	txdFmt.mp_impFunc = chc_tex_import_img;
	txdFmt.mp_expFunc = chc_tex_export_img;
	mp_fileFormats[1] = (FileFormat *)malloc(sizeof(FileFormat));
	memcpy(mp_fileFormats[1], &txdFmt, sizeof(FileFormat));


	memset(&txdFmt, 0, sizeof(txdFmt));
	txdFmt.description = "Keyframe";
	txdFmt.name = "kf";
	txdFmt.mp_impFunc = chc_engine_import_keyframe_collection;
	txdFmt.mp_expFunc = chc_engine_export_keyframe_collection;
	mp_fileFormats[2] = (FileFormat *)malloc(sizeof(FileFormat));
	memcpy(mp_fileFormats[2], &txdFmt, sizeof(FileFormat));

	memset(&txdFmt, 0, sizeof(txdFmt));
	txdFmt.description = "Collision";
	txdFmt.name = "col";
	txdFmt.mp_impFunc = chc_engine_import_mesh;
	txdFmt.mp_expFunc = chc_engine_export_mesh;
	mp_fileFormats[3] = (FileFormat *)malloc(sizeof(FileFormat));
	memcpy(mp_fileFormats[3], &txdFmt, sizeof(FileFormat));
}