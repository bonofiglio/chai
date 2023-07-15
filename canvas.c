#include "canvas.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "colors.h"
#include "utils.h"
#include "xchar.h"

size_t get_pixel_index(const size_t total_columns, const size_t row,
                       const size_t col) {
    return total_columns * row + col;
}

Canvas Canvas_new(const size_t height, const size_t width) {
    const size_t chars_len = width * height;
    const size_t str_buf_len = chars_len * MAX_XCHAR_STR_SIZE;
    Canvas c = {.width = width,
                .height = height,
                .chars = malloc(chars_len * sizeof(XChar)),
                .chars_len = chars_len,
                .str_buf = malloc(str_buf_len),
                .str_buf_len = str_buf_len};

    return c;
}

void Canvas_clear(Canvas* self) {
    for (size_t i = 0; i < self->height; i++) {
        for (size_t j = 0; j < self->width; j++) {
            self->chars[get_pixel_index(self->width, i, j)] =
                XChar_from_char(' ');
        }
    }

    for (size_t i = 0; i < self->str_buf_len; i++) {
        self->str_buf[i] = '\0';
    }
}

CharSizeTuple Canvas_to_str(Canvas* self) {
    size_t str_i = 0;

    for (size_t char_i = 0; char_i < self->chars_len; char_i++, str_i++) {
        const XChar* current = &self->chars[char_i];
        if (current->color_str) {
            for (size_t j = 0; j < COLOR_STR_SIZE; j++, str_i++) {
                self->str_buf[str_i] = current->color_str[j];
            }

            self->str_buf[str_i + 1] = current->c;
            str_i += 1;

            if (current->color_str != self->chars[char_i + 1].color_str) {
                str_i += 1;
                for (size_t j = 0; j < RESET_STR_SIZE; j++, str_i++) {
                    self->str_buf[str_i] = RESET[j];
                }
            }
        } else {
            self->str_buf[str_i] = current->c;
        }
    }

    CharSizeTuple result = {.size = str_i, .chars = self->str_buf};

    return result;
}

void Canvas_set_px(Canvas* self, const size_t x, const size_t y,
                   const XChar val) {
    self->chars[y * self->width + x] = val;
}

void Canvas_free(Canvas* self) {
    free(self->chars);
    free(self->str_buf);
}
