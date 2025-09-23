#pragma once
#include <stdint.h>
void getJPEG(uint8_t *resultBuffer, int *resultSize);
#define JPEG_BUFFER_SIZE 25000 //25kb
#ifdef __cplusplus
enum PacketType {
    PACKET_LOG,
    PACKET_VIDEO,
    PACKET_SOUND,
    PACKET_NETWORK,
    PACKET_INPUT
};
void cobsEncode(uint8_t* data, int &len, PacketType type);
#endif
extern const int COBS_OFFSET; // to account for COBS overhead
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