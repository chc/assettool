#ifndef _CRC32_H
#define _CRC32_H
#include <stdint.h>
uint32_t crc32(uint32_t crc, const void *buf, int size);
uint32_t crc32_extended(const void *buf, int size, bool case_insensitive = true, bool replace_spaces = true);
#endif
