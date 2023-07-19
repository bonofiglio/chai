#ifndef _CURSOR_H
#define _CURSOR_H

#include "utils.h"

typedef struct Cursor {
    size_t v_row;
    size_t v_col;
    size_t p_row;
    size_t p_col;
    size_t min_row;
    size_t min_col;
    size_t max_row;
    size_t max_col;
    char _str_buf[10];
} Cursor;

void Cursor_move(Cursor* self, const Directions dir);
Cursor Cursor_new(const size_t min_row, const size_t min_col,
                  const size_t max_row, const size_t max_col);
char* Cursor_to_str(Cursor* self);
ActiveCollisions Cursor_touching_border(Cursor* self);
void Cursor_set_min_row(Cursor* self, const size_t new_min);
void Cursor_set_min_col(Cursor* self, const size_t new_min);
void Cursor_set_max_row(Cursor* self, const size_t new_max);
void Cursor_set_max_col(Cursor* self, const size_t new_max);

#endif
