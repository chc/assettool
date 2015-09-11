#include <stdio.h>
#include <stdlib.h>
int file_len(FILE *fd);
FILE* slice_file(const char *name, const char *outname, int offset, int slice_len);