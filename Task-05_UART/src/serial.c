#include "serial.h"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

int serial_open(const char *path)
{
    int fd = open(path, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        return -1;
    }

    //Configs for UART
    
    struct termios settings;

    settings.c_cflag &= ~CSIZE;
    settings.c_cflag |= CS8;

    settings.c_cflag &= ~PARENB;
    settings.c_cflag &= ~CSTOPB;

    settings.c_cflag |= CREAD | CLOCAL;

    if (tcgetattr(fd, &settings) == -1) {
        int error = errno;
        close(fd);
        errno = error;
        return -1;
    }

    if (cfsetispeed(&settings, B115200) == -1) {
    return -1;
    }

    if (cfsetospeed(&settings, B115200) == -1) {
        return -1;
    }

    if (tcsetattr(fd, TCSANOW, &settings) == -1) {
        return -1;
    }
    return fd;
}

int serial_close(int fd)
{
    return close(fd);
}
