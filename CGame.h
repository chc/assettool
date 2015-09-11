#ifndef _IGAME_H
#define _IGAME_H
#include <stdio.h>
enum FileFormatFlags {
	FileFormat_DirContainer,
};
enum FileType {
	FileType_Texture, //CTexture interface
	FileType_FileContainer,
};
typedef struct {
	const char *path;
	const char *srcPath;
	FileType type; //FileType_Texture means void *dataClass = CImage *
	const char *args;
	void *dataClass;
} ExportOptions;
typedef bool (*ExportFunc)(ExportOptions *opts);
typedef struct {
	const char *path;
	const char *args;
	const char *outpath;
	const char *expArgs;
	ExportFunc exporter;
}ImportOptions;
typedef bool (*ImportFunc)(ImportOptions* opts);
typedef struct {
	const char *name;
	const char *description;
	ImportFunc mp_impFunc;
	ExportFunc mp_expFunc;
} FileFormat;
class IGame {
public:
	IGame() : m_fileFmtCount(0),mp_fileFormats(NULL) {};
	const char *getShortName();
	const char *getName();
	FileFormat **getFormats(int &count);
protected:
	const char *mp_shortName;
	const char *mp_fullName;
	FileFormat** mp_fileFormats;
	int			m_fileFmtCount;
};
#endif //_IGAME_H