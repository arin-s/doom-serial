#ifndef LINUX_UART
#define LINUX_UART

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

int openSerial(char *port);
void readSerial(uint8_t* &buf, int &size);
void writeSerial(unsigned char* buf, int size);
void closeSerial();

#endif