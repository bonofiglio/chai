#include "fs.h"
#include "screen.h"
#include "utils.h"

#ifndef _TERM_H
#define _TERM_H

#define ESC_SEQ_SPACE 100
#define SCREEN_REFRESH ESC "[2J" ESC "[H"
#define Option(type) int | void

typedef struct Cursor {
    size_t row;
    size_t col;
    size_t min_row;
    size_t min_col;
    char _str_buf[10];
} Cursor;

Cursor Cursor_new(const size_t min_col, const size_t min_row);
char* Cursor_to_str(Cursor* self);

typedef struct TermState {
    Cursor cursor;
    File* current_file;
} TermState;

Result get_window_size(size_t* rows, size_t* cols);

void enable_raw_mode();

void init_default_attrs();

void restore_term();

void setup_term();

Result get_cursor_position(Cursor* cursor);

#endif
