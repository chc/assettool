#include <main.h>
#include <XML/CCHCMax.h>
#include "CCHCMaxXML.h"
CCHCMax::CCHCMax() {
	FileFormat txdFmt;
	memset(&txdFmt,0,sizeof(txdFmt));
	txdFmt.description = "CHC XML format";
	txdFmt.name = "xml";
	txdFmt.mp_impFunc = chc_max_xml_import;
	txdFmt.mp_expFunc = chc_max_xml_export;

	m_fileFmtCount = 1;

	mp_shortName = "CHCMax";
	mp_fullName = "CHC Max";
	mp_fileFormats = (FileFormat **)malloc(sizeof(FileFormat *) * m_fileFmtCount);

	mp_fileFormats[0] = (FileFormat *)malloc(sizeof(FileFormat));
	memcpy(mp_fileFormats[0],&txdFmt,sizeof(FileFormat));
}