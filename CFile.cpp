#include "CFile.h"

void CFile::setSize(uint32_t bytes) {
	m_file_size = bytes;
}
uint32_t CFile::getSize() {
	return m_file_size;
}
void *CFile::getData() {
	return m_data;
}
void CFile::setData(void *data) {
	m_data = data;
}
char *CFile::getName() {
	return m_name;
}
void CFile::setName(char *name) {
	m_name = name;
}