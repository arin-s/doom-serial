#pragma once
#include <stdint.h>
void getJPEG(uint8_t *resultBuffer, int *resultSize);
static const int JPEG_BUFFER_SIZE = 25000; //25kb
unsigned int processInput(unsigned char *buf, unsigned int len);
#ifdef __cplusplus
extern "C" {
#endif
void* doom_malloc_log(int size, const char* file, const int line);
void doom_free_log(void* ptr, const char* file, const int line);
#define doom_malloc(size) doom_malloc_log(size, __FILE__, __LINE__)
#define doom_free(ptr) doom_free_log(ptr, __FILE__, __LINE__)
#ifdef __cplusplus
}
#endif