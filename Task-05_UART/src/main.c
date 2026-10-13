#include <stdio.h>
#include "serial.h"
#include "protocol.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <serial-device>\n", argv[0]);
        return 2;
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