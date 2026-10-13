#ifndef SERIAL_H
#define SERIAL_H

int serial_open(const char *path);
int serial_close(int fd);

#endif