#ifndef SERIAL_H
#define SERIAL_H

#include <sys/types.h>
#include <stddef.h>

ssize_t serial_read_byte(int fd, unsigned char *byte);

int serial_open(const char *path);
int serial_close(int fd);
int serial_write_all(int fd, const unsigned char *data, size_t length);

#endif