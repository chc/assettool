#include <FileSearcher.h>
#include <windows.h>
FileSearcher::FileSearcher(const char *path, FileSearcherCB callback, void *params) {
	strcpy(m_path, path);
	mp_cb_onfound = callback;
	mp_params = params;
}
void FileSearcher::run(const char *path) {
	const char *_path = (const char *)&m_path;
	if(path) {
		_path = path;
	}
	FileInfo file_info;
   	WIN32_FIND_DATA FindFileData;
   	HANDLE hFind;
   	char the_path[MAX_PATH+256];
   	sprintf(the_path,"%s\\*");
   	hFind = FindFirstFile(the_path,&FindFileData);
	do {
		file_info.name = (char *)&FindFileData.cFileName;
		if(mp_cb_onfound && ~FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			mp_cb_onfound(&file_info, mp_params);
		} else if(file_info.name[0] != '.') { //skip cur dir, hidden dirs, etc
				run(file_info.name);
			}
	} while(FindNextFile(hFind, &FindFileData) != 0);

	FindClose(hFind);
}
