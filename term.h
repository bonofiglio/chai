#ifndef _TERM_H
#define _TERM_H

#include <stddef.h>

#include "canvas.h"
#include "cursor.h"
#include "fs.h"

#define ESC_SEQ_SPACE 100
#define SCREEN_REFRESH ESC "[2J" ESC "[H"
#define Option(type) int | void

typedef struct TermState {
    File* current_file;
    size_t scroll_pos;
    Canvas canvas;
    Cursor cursor;
} TermState;

TermState TermState_new(Canvas canvas, Cursor cursor);
void TermState_scoll(TermState* self, enum Directions dir,
                     const size_t max_height);
void TermState_free(TermState* self);
size_t TermState_get_text_row_height(TermState* self, const size_t idx,
                                     const size_t padding);
size_t TermState_get_text_row_at(TermState* self, const size_t y,
                                 const size_t padding);

Result get_window_size(size_t* rows, size_t* cols);

void enable_raw_mode();

void init_default_attrs();

void restore_term();

void setup_term();

Result get_cursor_position(Cursor* cursor);

#endif
