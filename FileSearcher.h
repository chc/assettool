#ifndef _FILESEARCHER_H
#define _FILESEARCHER_H
#include <main.h>
typedef struct {
	char *name;
} FileInfo;
typedef void (*FileSearcherCB)(FileInfo *, void *);
class FileSearcher {
	public:
		FileSearcher(const char *path, FileSearcherCB callback, void *params);
		void run(const char *path = NULL);
	private:
		char m_path[FILENAME_MAX+1];
		FileSearcherCB mp_cb_onfound;
		void *mp_params;
};

#endif
