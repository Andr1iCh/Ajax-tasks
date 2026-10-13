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