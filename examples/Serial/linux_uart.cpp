// Linux target
// Most of the code is from this excellent source: https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/

// Linux headers
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <errno.h>   // Error integer and strerror() function
#include <unistd.h>  // write(), read(), close()
#include <sys/ioctl.h> // Used for TCGETS2/TCSETS2, which is required for custom baud rates
#include <asm/termbits.h> // for termios2

#include <string.h>
#include <stdio.h>

#include <thread>
#include <mutex>
#include <atomic> // For shared pointer and kill flag
#include <climits>


/*
A simpler consumer-producer variant:
1. The consumer consumes far slower than the producer produces.
2. The frames produced by the producer become stale the moment the next frame is generated.
Thus, there's no point queueing any frame other than the most recently produced one, making the implementation pretty simple.
*/

std::thread consumer_thread;
std::mutex buffer_lock;
uint8_t* shared_buffer_pointer;
int shared_buffer_size;
std::atomic<bool> shared_stop_flag = false;
int serial_port;
void consumer_func();

int openSerial(char *port)
{
    // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)
    serial_port = open(port, O_RDWR);
    usleep(1000);
    ioctl(serial_port, TCFLSH, 2); // flush both

    // Create termios2 struct so we can set a custom baud rate
    struct termios2 tty;

    // Read in existing settings, and handle any error
    if (ioctl(serial_port, TCGETS2, &tty) < 0)
    {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    tty.c_cflag &= ~PARENB;        // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB;        // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE;         // Clear all bits that set the data size
    tty.c_cflag |= CS8;            // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS;       // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;                                                        // Disable echo
    tty.c_lflag &= ~ECHOE;                                                       // Disable erasure
    tty.c_lflag &= ~ECHONL;                                                      // Disable new-line echo
    tty.c_lflag &= ~ISIG;                                                        // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);                                      // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    tty.c_cflag &= ~CBAUD;
    tty.c_cflag |= CBAUDEX;
    tty.c_ispeed = 3000000; // Set in/out baud rate to be 3M
    tty.c_ospeed = 3000000;

    // for read()
    tty.c_cc[VTIME] = 0; // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Save tty settings, also checking for error
    if (ioctl(serial_port, TCSETS2, &tty) < 0)
    {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return -1;
    }

    // Initialise the shared buffer
    shared_buffer_pointer = new uint8_t[25000];
    // Start the consumer thread
    consumer_thread = std::thread(consumer_func);
    return serial_port;
}

void writeSerial(uint8_t* buf, int size) {
    // obtain lock
    buffer_lock.lock();
    // write data to shared buffer
    memcpy(shared_buffer_pointer, buf, size);
    shared_buffer_size = size;
    // release lock
    buffer_lock.unlock();
}

void closeSerial() {
  close(serial_port);
  shared_stop_flag = true;
  consumer_thread.join();
  delete[] shared_buffer_pointer;
}

// Consumer thread function
void consumer_func() {
    // stop it from spamming stdout on startup
    shared_buffer_size = -1;
    bool game_started = false;
    // internal write buffer
    uint8_t* write_buffer = new uint8_t[25000];
    int size = 0;
    while(shared_stop_flag == false) {
        // obtain lock
        buffer_lock.lock();
        // if we already read this frame
        if(shared_buffer_size == -1) {
            if(game_started) {
                printf("The game is running behind! (or starting up)\n");
            }
        }
        else {
            game_started = true;
            // copy shared buffer to write buffer
            memcpy(write_buffer, shared_buffer_pointer, shared_buffer_size);
            // save buffer size
            size = shared_buffer_size;
            // make frame stale
            shared_buffer_size = -1;
        }
        buffer_lock.unlock();
        write(serial_port, write_buffer, size);
    }
    delete[] write_buffer;
}