#include "serial.h"
#include <fcntl.h>
#include <unistd.h>

int serial_open(const char *path)
{
    return open(path, O_RDWR | O_NOCTTY);
}

int serial_close(int fd)
{
    return close(fd);
}
