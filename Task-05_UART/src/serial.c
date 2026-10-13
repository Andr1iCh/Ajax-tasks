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

    struct termios settings;
    if (tcgetattr(fd, &settings) == -1) {
        int error = errno;
        close(fd);
        errno = error;
        return -1;
    }

    return fd;
}

int serial_close(int fd)
{
    return close(fd);
}
