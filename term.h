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

int mut_write_screenbuffer(ScreenBuffer* mut_self, char* data, unsigned int bytes);
void mut_clear_screenbuffer(ScreenBuffer* mut_self);
void mut_resize_screenbuffer(ScreenBuffer* mut_self, unsigned int new_size);
void mut_free_screenbuffer(ScreenBuffer* mut_self);

typedef struct TermState {
    Screen screen;
    ScreenBuffer screen_buffer;
    Cursor cursor;
} TermState;

int mut_get_window_size(int* mut_rows, int* mut_cols);

void enable_raw_mode();

void init_default_attrs();

void restore_term();

void setup_term();

int mut_get_cursor_position(Cursor* mut_cursor);

#endif
