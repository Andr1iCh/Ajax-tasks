#include <stdio.h>
#include "serial.h"

int main (int argc, char *argv[]){
    if(argc!=2){
        fprintf(stderr,"Usage: %s <serial-device>\n", argv[0]);
        return 2;
    }

    int port_fd = serial_open(argv[1]);
    if(port_fd == -1){
        perror(argv[1]);
        return 1;
    }

    printf("Opened %s (fd=%d)\n", argv[1], port_fd);
    

    unsigned char byte;
    ssize_t count = serial_read_byte(port_fd, &byte);

    if (count != 1) {
        if (count == -1) {
            perror("read serial port");
        } else {
            fprintf(stderr, "Serial port closed while reading\n");
        }

        serial_close(port_fd);
        return 1;
    }

    printf("Received byte: 0x%02X\n", (unsigned int)byte);

    if (serial_close(port_fd) == -1) {
    perror("close serial port");
    return 1;
    }
    
    return 0;
}