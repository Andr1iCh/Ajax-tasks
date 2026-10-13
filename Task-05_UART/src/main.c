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
    
    if (serial_close(port_fd) == -1) {
    perror("close serial port");
    return 1;
    }
    
    return 0;
}