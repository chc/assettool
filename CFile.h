#ifndef _CFILE_H
#define _CFILE_H
#include <stdint.h>
class CFile {
public:
	void setSize(uint32_t bytes);
	uint32_t getSize();
	void *getData();
	void setData(void *data);
	char *getName();
	void setName(char *name);
protected:
	uint32_t m_file_size;
	char *m_name;
	void *m_data;
};
#endif //_CFILE_H