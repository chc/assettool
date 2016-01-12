#ifndef _FILESEARCHER_H
#define _FILESEARCHER_H

typedef struct {
	char *name;
} FileInfo;
typedef void (*FileSearcherCB)(FileInfo *, void *);
class FileSearcher {
	public:
		FileSearcher(const char *path, FileSearcherCB callback, void *params);
		void run();
	private:
		const char *mp_path;
		FileSearcherCB mp_cb_onfound;
		void *mp_params;
};

#endif
