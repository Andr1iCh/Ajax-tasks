#define _POSIX_C_SOURCE 200809L

#include "protocol.h"
#include "serial.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

static volatile sig_atomic_t stop_requested = 0;

static FILE *open_log_file(void);
static int log_received(FILE *log_file, const LineBuffer *line);
static void log_error(FILE *log_file, const char *message, int error_code);

static void handle_stop_signal(int signal_number) {
  (void)signal_number;
  stop_requested = 1;
}

static int install_signal_handlers(void) {
  struct sigaction action = {0};

  action.sa_handler = handle_stop_signal;
  action.sa_flags = 0;

  if (sigemptyset(&action.sa_mask) == -1) {
    return -1;
  }

  if (sigaction(SIGINT, &action, NULL) == -1) {
    return -1;
  }

  if (sigaction(SIGTERM, &action, NULL) == -1) {
    return -1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <serial-device>\n", argv[0]);
    return 2;
  }

  if (install_signal_handlers() == -1) {
    perror("install signal handlers");
    return 1;
  }
  FILE *log_file = open_log_file();
  if (log_file == NULL) {
    return 1;
  }

  int port_fd = serial_open(argv[1]);
  if (port_fd == -1) {
    int saved_errno = errno;
    log_error(log_file, argv[1], saved_errno);

    if (fclose(log_file) == EOF) {
      perror("close log file");
    }

    return 1;
  }

  printf("Opened %s (fd=%d)\n", argv[1], port_fd);

  LineBuffer line = {0};
  int exit_code = 0;

  while (!stop_requested) {
    unsigned char byte;
    ssize_t count = serial_read_byte(port_fd, &byte);

    if (stop_requested) {
      break;
    }

    if (count == -1 && errno == EINTR) {
      continue;
    }

    if (count == 1) {
      LineResult state = line_feed(&line, byte);

      if (state == LINE_READY) {
        if (log_received(log_file, &line) == -1) {
          exit_code = 1;
          break;
        }
        line_swap_case(&line);

        if (serial_write_all(port_fd, line.data, line.length) == -1 ||
            serial_write_all(port_fd, (const unsigned char *)"\r\n", 2) == -1) {
          int saved_errno = errno;
          log_error(log_file, "write serial port", saved_errno);
          exit_code = 1;
          break;
        }

        printf("Sent response (%zu bytes): ", line.length);
        fwrite(line.data, 1, line.length, stdout);
        putchar('\n');

        line_reset(&line);
      } else if (state == LINE_TOO_LONG) {
        log_error(log_file, "Input line is too long", 0);
        exit_code = 1;
        break;
      }

      continue;
    }

    if (count == 0) {
      log_error(log_file, "Serial port closed while reading", 0);
    } else {
      int saved_errno = errno;
      log_error(log_file, "read serial port", saved_errno);
    }

    exit_code = 1;
    break;
  }

  if (stop_requested) {
    printf("\nShutdown requested\n");

    if (fprintf(log_file, "INFO: Shutdown requested\n") < 0) {
      perror("write shutdown to log");
      exit_code = 1;
    }
  }

  if (serial_close(port_fd) == -1) {
    int saved_errno = errno;
    log_error(log_file, "close serial port", saved_errno);
    exit_code = 1;
  }

  if (fclose(log_file) == EOF) {
    perror("close log file");
    exit_code = 1;
  }

  return exit_code;
}

static FILE *open_log_file(void) {
  if (mkdir("logs", 0755) == -1 && errno != EEXIST) {
    perror("create logs directory");
    return NULL;
  }

  time_t started_at = time(NULL);
  if (started_at == (time_t)-1) {
    fprintf(stderr, "Cannot get start time\n");
    return NULL;
  }

  struct tm *started_tm = localtime(&started_at);
  if (started_tm == NULL) {
    fprintf(stderr, "Cannot convert start time\n");
    return NULL;
  }

  char log_path[64];
  if (strftime(log_path, sizeof(log_path), "logs/uart_%Y-%m-%d_%H-%M-%S.log",
               started_tm) == 0) {
    fprintf(stderr, "Cannot format log filename\n");
    return NULL;
  }

  FILE *log_file = fopen(log_path, "a");
  if (log_file == NULL) {
    perror(log_path);
    return NULL;
  }

  printf("Log file: %s\n", log_path);
  return log_file;
}

static int log_received(FILE *log_file, const LineBuffer *line) {
  time_t received_at = time(NULL);
  if (received_at == (time_t)-1) {
    fprintf(stderr, "Cannot get message time\n");
    return -1;
  }

  struct tm *received_tm = localtime(&received_at);
  if (received_tm == NULL) {
    fprintf(stderr, "Cannot convert message time\n");
    return -1;
  }

  char timestamp[32];
  if (strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S",
               received_tm) == 0) {
    fprintf(stderr, "Cannot format message timestamp\n");
    return -1;
  }

  if (fprintf(log_file, "[%s] RX: ", timestamp) < 0 ||
      fwrite(line->data, 1, line->length, log_file) != line->length ||
      fputc('\n', log_file) == EOF || fflush(log_file) == EOF) {
    perror("write received message to log");
    return -1;
  }

  return 0;
}

static void log_error(FILE *log_file, const char *message, int error_code) {
  char timestamp[32] = "unknown time";

  time_t now = time(NULL);
  if (now != (time_t)-1) {
    struct tm *local = localtime(&now);
    if (local != NULL) {
      if (strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local) ==
          0) {
        strcpy(timestamp, "unknown time");
      }
    }
  }

  const char *details = error_code != 0 ? strerror(error_code) : NULL;

  int result;

  if (details != NULL) {
    fprintf(stderr, "%s: %s\n", message, details);
    result =
        fprintf(log_file, "[%s] ERROR: %s: %s\n", timestamp, message, details);
  } else {
    fprintf(stderr, "%s\n", message);
    result = fprintf(log_file, "[%s] ERROR: %s\n", timestamp, message);
  }

  if (result < 0 || fflush(log_file) == EOF) {
    fprintf(stderr, "Cannot write error to log\n");
  }
}