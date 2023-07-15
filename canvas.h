#ifndef _CANVAS_H
#define _CANVAS_H

#include <stddef.h>

#include "xchar.h"

typedef struct CharSizeTuple {
    char* chars;
    size_t size;
} CharSizeTuple;

typedef struct Canvas {
    size_t width;
    size_t height;
    XChar* chars;
    size_t chars_len;
    char* str_buf;
    size_t str_buf_len;
} Canvas;

Canvas Canvas_new(const size_t height, const size_t width);
void Canvas_clear(Canvas* self);
CharSizeTuple Canvas_to_str(Canvas* self);
void Canvas_set_px(Canvas* self, const size_t x, const size_t y,
                   const XChar val);
void Canvas_free(Canvas* self);

#endif
