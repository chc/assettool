#include <FileSearcher.h>
#include <dirent.h>
FileSearcher::FileSearcher(const char *path, FileSearcherCB callback, void *params) {
	strcpy(m_path, path);
	mp_cb_onfound = callback;
	mp_params = params;
}
void FileSearcher::run() {
	DIR *dir = opendir(m_path);
	struct dirent *dent;
	FileInfo file_info;
	if(dir != NULL) {
		while((dent=readdir(dir)) != NULL) {
			file_info.name = dent->d_name;
			if(mp_cb_onfound) {
				mp_cb_onfound(&file_info, mp_params);
			}
		}
	}
	closedir(dir);
}