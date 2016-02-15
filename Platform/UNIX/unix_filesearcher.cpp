#include <FileSearcher.h>
#include <dirent.h>
FileSearcher::FileSearcher(const char *path, FileSearcherCB callback, void *params) {
	strcpy(m_path, path);
	mp_cb_onfound = callback;
	mp_params = params;
}
void FileSearcher::run(const char *path) {
	const char *the_path = (const char *)&m_path;
	if(path) {
		the_path = path;
	}
	DIR *dir = opendir(the_path);
	struct dirent *dent;
	char filebuf[FILENAME_MAX+1];
	FileInfo file_info;
	if(dir != NULL) {
		while((dent=readdir(dir)) != NULL) {
			sprintf(filebuf, "%s/%s",the_path,dent->d_name);
			file_info.name = (char *)&filebuf;
			if(mp_cb_onfound && dent->d_type != DT_DIR) {
				mp_cb_onfound(&file_info, mp_params);
			} else if(dent->d_name[0] != '.') { //skip cur dir, hidden dirs, etc
				run(file_info.name);
			}
		}
	}
	closedir(dir);
}
