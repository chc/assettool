#include <main.h>
#include <Generic/CGeneric.h>
#include <Generic/CImage.h>
#include "pngExporter.h"
#include "fileExport.h"
#include "plyloader.h"
#include "TTF.h"
#include "Cubemap.h"
#include "Json.h"
CGeneric::CGeneric() {
	FileFormat formats[] = {{"png","PNG",png_import_img,png_export_img},
							{"fs", "File System",gen_import_file, gen_export_file},
							{"ply","PLY file", ply_import_mesh, ply_export_mesh},
							{"ttf", "TTF Font", ttf_import_mesh, ttf_export_mesh},
							{ "cube", "CubeMap", gen_import_cubemap, gen_export_cubemap },
							{ "jsonm", "JSON Mesh", gen_import_json_mesh, gen_export_json_mesh },
	};

	m_fileFmtCount = sizeof(formats)/sizeof(FileFormat);

	mp_shortName = "Generic";
	mp_fullName = "Generic File Formats";
	mp_fileFormats = (FileFormat **)malloc(sizeof(FileFormat *) * m_fileFmtCount);
	for(int i=0;i<m_fileFmtCount;i++) {
		mp_fileFormats[i] = (FileFormat*)malloc(sizeof(FileFormat));
		memcpy(mp_fileFormats[i], &formats[i], sizeof(FileFormat));
	}

}