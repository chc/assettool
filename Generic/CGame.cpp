#include <Generic/CGame.h>
#include <stdlib.h>
IGame::~IGame() {
	for(int i=0;i<m_fileFmtCount;i++) {
		free((void *)mp_fileFormats[i]);
	}
	free(mp_fileFormats);
}
const char *IGame::getShortName() {
	return mp_shortName;
}
const char *IGame::getName() {
	return mp_fullName;
}
FileFormat **IGame::getFormats(int &count) {
	count = m_fileFmtCount;
	return mp_fileFormats;
}