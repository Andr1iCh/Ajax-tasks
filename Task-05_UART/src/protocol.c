#include "protocol.h"

LineResult line_feed(LineBuffer *line, unsigned char byte)
{
    if (byte == '\n' &&
        line->length > 0 &&
        line->data[line->length - 1] == '\r') {
        --line->length; /* прибрати \r із вмісту повідомлення */
        return LINE_READY;
    }

    if (line->length == LINE_CAPACITY) {
        return LINE_TOO_LONG;
    }

    line->data[line->length++] = byte;
    return LINE_MORE;
}

void line_reset(LineBuffer *line)
{
    line->length = 0;
}

void line_swap_case(LineBuffer *line)
{
    for (size_t i = 0; i < line->length; ++i) {
        unsigned char byte = line->data[i];

        if (byte >= 'a' && byte <= 'z') {
            line->data[i] = byte - 'a' + 'A';
        } else if (byte >= 'A' && byte <= 'Z') {
            line->data[i] = byte - 'A' + 'a';
        }
    }
}