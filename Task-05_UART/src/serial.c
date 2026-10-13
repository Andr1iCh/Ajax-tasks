
    
  
#include "serial.h"
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int serial_open(const char *path) {
  int fd = open(path, O_RDWR | O_NOCTTY);
  if (fd == -1) {
    return -1;
  }

  // Configs for UART

  struct termios settings;

  if (tcgetattr(fd, &settings) == -1) {
    int error = errno;
    close(fd);
    errno = error;
    return -1;
  }

  settings.c_cflag &= ~CSIZE;
  settings.c_cflag |= CS8;

  settings.c_cflag &= ~PARENB;
  settings.c_cflag &= ~CSTOPB;

  settings.c_cflag |= CREAD | CLOCAL;

  settings.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                        ICRNL | IXON | IXOFF | IXANY);
  settings.c_oflag &= ~OPOST;
  settings.c_lflag &= ~(ICANON | ECHO | ECHONL | ISIG | IEXTEN);

  settings.c_cc[VMIN] = 1;
  settings.c_cc[VTIME] = 0;

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

int serial_close(int fd) { return close(fd); }

ssize_t serial_read_byte(int fd, unsigned char *byte) {
  return read(fd, byte, 1);
}

int serial_write_all(int fd, const unsigned char *data, size_t length) {
  size_t sent = 0;

  while (sent < length) {
    ssize_t count = write(fd, data + sent, length - sent);

    if (count > 0) {
      sent += (size_t)count;
    } else if (count == -1 && errno == EINTR) {
      continue;
    } else {
      if (count == 0) {
        errno = EIO;
      }
      return -1;
    }
  }

  return 0;
}