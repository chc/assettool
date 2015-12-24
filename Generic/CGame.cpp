#include <Generic/CGame.h>

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