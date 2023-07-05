#ifndef _TERM_H
#define _TERM_H

#include "utils.h"

#define ESC_SEQ_SPACE 100
#define SCREEN_REFRESH ESC "[2J" ESC "[H"

typedef struct Screen {
    int rows;
    int columns;
} Screen;

typedef struct ScreenBuffer {
    char* b;
    unsigned int size;
    unsigned int current_index;
} ScreenBuffer;

typedef struct Cursor {
    int row;
    int col;
} Cursor;

void ScreenBuffer_write(ScreenBuffer* self, const char* data, const unsigned int bytes);
void ScreenBuffer_clear(ScreenBuffer* self);
void ScreenBuffer_resize(ScreenBuffer* self, unsigned int new_size);
void ScreenBuffer_free(ScreenBuffer* self);

typedef struct TermState {
    Screen screen;
    ScreenBuffer screen_buffer;
    Cursor cursor;
} TermState;

Result get_window_size(int* rows, int* cols);

void enable_raw_mode();

void init_default_attrs();

void restore_term();

void setup_term();

Result get_cursor_position(Cursor* cursor);

#endif
