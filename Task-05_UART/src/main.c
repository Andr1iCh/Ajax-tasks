#include <stdio.h>
#include "serial.h"
#include "protocol.h"
#include <errno.h>
#include <sys/stat.h>
#include <time.h>

static FILE *open_log_file(void);
static int log_received(FILE *log_file, const LineBuffer *line);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <serial-device>\n", argv[0]);
        return 2;
    }

    FILE *log_file = open_log_file();
    if (log_file == NULL) {
        return 1;
    }

    int port_fd = serial_open(argv[1]);
    if (port_fd == -1) {
        perror(argv[1]);
        return 1;
    }

    printf("Opened %s (fd=%d)\n", argv[1], port_fd);

    LineBuffer line = {0};
    int exit_code = 0;

    for (;;) {
        unsigned char byte;
        ssize_t count = serial_read_byte(port_fd, &byte);

        if (count == 1) {
            LineResult state = line_feed(&line, byte);

            if (state == LINE_READY) {
                if (log_received(log_file, &line) == -1) {
                    exit_code = 1;
                    break;
                }
                line_swap_case(&line);

                if (serial_write_all(port_fd, line.data, line.length) == -1 ||
                    serial_write_all(port_fd,
                                    (const unsigned char *)"\r\n", 2) == -1) {
                    perror("write serial port");
                    exit_code = 1;
                    break;
                }

                printf("Sent response (%zu bytes): ", line.length);
                fwrite(line.data, 1, line.length, stdout);
                putchar('\n');

                line_reset(&line);
            } else if (state == LINE_TOO_LONG) {
                fprintf(stderr, "Input line is too long\n");
                exit_code = 1;
                break;
            }

            continue;
        }

        if (count == 0) {
            fprintf(stderr, "Serial port closed while reading\n");
        } else {
            perror("read serial port");
        }

        exit_code = 1;
        break;
    }

    if (serial_close(port_fd) == -1) {
        perror("close serial port");
        return 1;
    }

    return exit_code;
}

static FILE *open_log_file(void)
{
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
    if (strftime(log_path, sizeof(log_path),
                 "logs/uart_%Y-%m-%d_%H-%M-%S.log",
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

static int log_received(FILE *log_file, const LineBuffer *line)
{
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
    if (strftime(timestamp, sizeof(timestamp),
                 "%Y-%m-%d %H:%M:%S", received_tm) == 0) {
        fprintf(stderr, "Cannot format message timestamp\n");
        return -1;
    }

    if (fprintf(log_file, "[%s] RX: ", timestamp) < 0 ||
        fwrite(line->data, 1, line->length, log_file) != line->length ||
        fputc('\n', log_file) == EOF ||
        fflush(log_file) == EOF) {
        perror("write received message to log");
        return -1;
    }

    return 0;
}