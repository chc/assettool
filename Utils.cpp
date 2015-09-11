#include "Utils.h"
#include <stdio.h>
#include <stdlib.h>

int file_len(FILE *fd) {
	int pos = ftell(fd);
	fseek(fd,0,SEEK_END);
	int len = ftell(fd);
	fseek(fd,pos,SEEK_SET);
	return len;
}

FILE* slice_file(const char *name, const char *outname, int offset, int slice_len) {
	char tmpname[FILENAME_MAX+1];
	char *writebuf = NULL;
	FILE *fd = fopen(name,"rb");
	if(!fd) {
		return NULL;
	}
	sprintf(tmpname, "%s.tmp",outname);
	FILE *tmpfd = fopen(tmpname,"wb");
	if(!tmpfd) {
		fclose(fd);
		return NULL;
	}
	fseek(fd,0,SEEK_SET);
	int len = file_len(fd);
	writebuf = (char *)malloc(len);
	fread(writebuf,offset,1,fd);
	fwrite(writebuf,offset,1,tmpfd);
	fseek(fd,offset+slice_len,SEEK_SET);
	len -= offset+slice_len;
	fread(writebuf,len,1,fd);
	fwrite(writebuf,len,1,tmpfd);
	free(writebuf);
	fclose(fd);
	fclose(tmpfd);

	unlink(name);

	rename(tmpname,name);
	fd = fopen(name, "rb+");
	fseek(fd,0,SEEK_END);
	return fd;
}