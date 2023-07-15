#ifndef _XCHAR_H
#define _XCHAR_H

#include "colors.h"

#define MAX_XCHAR_STR_SIZE (sizeof(char) + COLOR_STR_SIZE + RESET_STR_SIZE)

typedef struct XChar {
    char c;
    const char* color_str;
    // Extend later with unicode support and rgb colors
} XChar;

XChar XChar_from_char(const char c);
XChar XChar_with_color(const char c, const char* color_str);

#endif
