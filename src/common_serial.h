#pragma once
#include <stdint.h>
void getJPEG(uint8_t *resultBuffer, int *resultSize);
static const int JPEG_BUFFER_SIZE = 25000; //25kb
void addKeyToQueue(int pressed, unsigned int keyCode);
