#include "xchar.h"

#include "stddef.h"

XChar XChar_from_char(const char c) {
    const XChar xc = {.c = c, .color_str = NULL};

    return xc;
}

XChar XChar_with_color(const char c, const char* color_str) {
    const XChar xc = {.c = c, .color_str = color_str};

    return xc;
}
