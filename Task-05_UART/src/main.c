#include <stdio.h>

int main (int argc, char *argv[]){
    if(argc!=2){
        fprintf(stderr,"Usage: %s <serial-device>\n", argv[0]);
        return 2;
    }

    printf("Serial device: %s\n", argv[1]);
    return 0;
}