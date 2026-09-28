#!/bin/bash
aarch64-linux-gnu-gcc -Wall -Wextra -O2 -static -mcpu=cortex-a76 main.c -o app_static_target