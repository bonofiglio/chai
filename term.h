#ifndef _TERM_H
#define _TERM_H

#include <stddef.h>

#include "canvas.h"
#include "cursor.h"
#include "fs.h"

#define ESC_SEQ_SPACE 100
#define SCREEN_REFRESH ESC "[2J" ESC "[H"
#define Option(type) int | void
typedef enum Mode { Normal, Insert } Mode;

typedef struct TermState {
    File* current_file;
    size_t scroll_pos;
    Canvas canvas;
    Cursor cursor;
    Mode mode;
    size_t* row_num_cache;
} TermState;

TermState TermState_new(Canvas canvas, Cursor cursor);
size_t TermState_get_scrolled_lines(TermState* self);
void TermState_scroll(TermState* self, enum Directions dir);
void TermState_free(TermState* self);
void TermState_gen_row_num_cache(TermState* self);
size_t TermState_get_text_row_height(TermState* self, const size_t idx);
size_t TermState_get_current_text_line_col(TermState* self);
size_t TermState_get_text_row_at(TermState* self, const size_t y);
void TermState_insert_char(TermState* self, const char k);

Result get_window_size(size_t* rows, size_t* cols);

void enable_raw_mode();

void init_default_attrs();

void restore_term();

void setup_term();

Result get_cursor_position(Cursor* cursor);

#endif
