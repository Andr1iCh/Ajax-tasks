#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>

#define LINE_CAPACITY 256

typedef struct {
    unsigned char data[LINE_CAPACITY];
    size_t length;
} LineBuffer;

typedef enum {
    LINE_MORE,
    LINE_READY,
    LINE_TOO_LONG
} LineResult;

LineResult line_feed(LineBuffer *line, unsigned char byte);
void line_reset(LineBuffer *line);

#endif