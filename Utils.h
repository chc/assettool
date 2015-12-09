#include <stdio.h>
#include <stdlib.h>
#include <string>
int file_len(FILE *fd);
FILE* slice_file(const char *name, const char *outname, int offset, int slice_len);
std::string tolower(std::string s);
int isPowerOfTwo (unsigned int x);