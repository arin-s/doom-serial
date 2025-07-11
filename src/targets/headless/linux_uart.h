#ifndef LINUX_UART
#define LINUX_UART

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

int openSerial(char *port);
void writeSerial(unsigned char* buf, int size);
void closeSerial();

#endif