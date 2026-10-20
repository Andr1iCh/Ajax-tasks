#define _DEFAULT_SOURCE

#include "serial.h"
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <unistd.h>

int serial_open(const char *path) {
  int fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);
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
  settings.c_cflag &= ~CRTSCTS;

  settings.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                        ICRNL | IXON | IXOFF | IXANY);
  settings.c_oflag &= ~OPOST;
  settings.c_lflag &= ~(ICANON | ECHO | ECHONL | ISIG | IEXTEN);

  settings.c_cc[VMIN] = 1;
  settings.c_cc[VTIME] = 0;

  if (cfsetispeed(&settings, B115200) == -1 ||
      cfsetospeed(&settings, B115200) == -1 ||
      tcsetattr(fd, TCSANOW, &settings) == -1) {
    int saved_errno = errno;
    close(fd);
    errno = saved_errno;
    return -1;
  }
  return fd;
}

int serial_close(int fd) { return close(fd); }

static int serial_wait(int fd, short events,
                       const volatile sig_atomic_t *stop) {
  struct pollfd port = {.fd = fd, .events = events};

  for (;;) {
    if (*stop) {
      errno = ECANCELED;
      return -1;
    }

    int result = poll(&port, 1, 100);

    if (result == -1) {
      if (errno == EINTR) {
        continue;
      }
      return -1;
    }

    if (result == 0) {
      continue;
    }

    if (port.revents & POLLNVAL) {
      errno = EBADF;
      return -1;
    }

    if (port.revents & events) {
      return 0;
    }

    if (port.revents & (POLLERR | POLLHUP)) {
      errno = EIO;
      return -1;
    }
  }
}

ssize_t serial_read_byte(int fd, unsigned char *byte,
                         const volatile sig_atomic_t *stop) {
  for (;;) {
    if (serial_wait(fd, POLLIN, stop) == -1) {
      return -1;
    }

    if (*stop) {
      errno = ECANCELED;
      return -1;
    }

    ssize_t count = read(fd, byte, 1);

    if (count == -1 &&
        (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)) {
      continue;
    }

    return count;
  }
}

int serial_write_all(int fd, const unsigned char *data, size_t length,
                     const volatile sig_atomic_t *stop) {
  size_t sent = 0;

  while (sent < length) {
    if (*stop) {
      errno = ECANCELED;
      return -1;
    }

    ssize_t count = write(fd, data + sent, length - sent);

    if (count > 0) {
      sent += (size_t)count;
    } else if (count == -1 && errno == EINTR) {
      continue;
    } else if (count == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      if (serial_wait(fd, POLLOUT, stop) == -1) {
        return -1;
      }
    } else {
      if (count == 0) {
        errno = EIO;
      }
      return -1;
    }
  }

  return 0;
}