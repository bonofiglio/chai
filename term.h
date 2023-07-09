#include "file.h"
#include "utils.h"

#ifndef _TERM_H
#define _TERM_H

#define ESC_SEQ_SPACE 100
#define SCREEN_REFRESH ESC "[2J" ESC "[H"

typedef struct Screen {
    size_t rows;
    size_t columns;
    size_t padding_x;
    size_t padding_y;
} Screen;

typedef struct ScreenBuffer {
    char* b;
    size_t size;
    size_t current_index;
} ScreenBuffer;

typedef struct Cursor {
    size_t row;
    size_t col;
} Cursor;

void ScreenBuffer_reset(ScreenBuffer* self);
void ScreenBuffer_write(ScreenBuffer* self, const char* data,
                        const size_t bytes);
void ScreenBuffer_clear(ScreenBuffer* self);
void ScreenBuffer_resize(ScreenBuffer* self, size_t new_size);
void ScreenBuffer_free(ScreenBuffer* self);

typedef struct TermState {
    Screen w_screen;
    Screen t_screen;
    ScreenBuffer screen_buffer;
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
