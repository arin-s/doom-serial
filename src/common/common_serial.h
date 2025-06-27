#pragma once
#include <stdint.h>
void getJPEG(uint8_t *resultBuffer, int *resultSize);
static const int JPEG_BUFFER_SIZE = 25000; //25kb
#ifdef __cplusplus
extern "C" {
#endif
extern unsigned char doom_wad_data_start[];
extern unsigned char doom_wad_data_end[];
#ifdef __cplusplus
} // end extern "C"
#endif