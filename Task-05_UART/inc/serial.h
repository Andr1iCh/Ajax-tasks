#ifndef SERIAL_H
#define SERIAL_H

#include <sys/types.h>
ssize_t serial_read_byte(int fd, unsigned char *byte);

int serial_open(const char *path);
int serial_close(int fd);

#endif