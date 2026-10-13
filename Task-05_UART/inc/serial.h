#ifndef SERIAL_H
#define SERIAL_H

#include <signal.h>
#include <stddef.h>
#include <sys/types.h>

ssize_t serial_read_byte(int fd, unsigned char *byte,
                         const volatile sig_atomic_t *stop);

int serial_open(const char *path);
int serial_close(int fd);
int serial_write_all(int fd, const unsigned char *data, size_t length,
                     const volatile sig_atomic_t *stop);

#endif