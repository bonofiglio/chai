#include "cursor.h"

Cursor Cursor_new(const size_t min_row, const size_t min_col,
                  const size_t max_row, const size_t max_col) {
    Cursor c = {.v_row = 0,
                .v_col = 0,
                .min_row = min_row,
                .min_col = min_col,
                .max_row = max_row,
                .max_col = max_col,
                ._str_buf = {'\0'}};

    return c;
}

char* Cursor_to_str(Cursor* self) {
    snprintf(self->_str_buf, sizeof(self->_str_buf), ESC "[%zu;%zuH",
             self->v_row + 1, self->v_col + 1);

    return self->_str_buf;
}

void Cursor_move(Cursor* self, const enum Directions dir) {
    switch (dir) {
        case Left: {
            if (self->v_col > self->min_col) {
                self->v_col--;
            } else if (self->v_row > self->min_row) {
                self->v_row--;
                self->v_col = self->max_col - 1;
            }
            break;
        }
        case Right: {
            if (self->v_col < self->max_col - 1) {
                self->v_col++;
            } else if (self->v_row < self->max_row - 1) {
                self->v_row++;
                self->v_col = self->min_col;
            }
            break;
        }
        case Down: {
            if (self->v_row >= self->max_row - 1) break;

            self->v_row++;
            break;
        }
        case Up: {
            if (self->v_row <= 0) break;

            self->v_row--;
            break;
        }
    }
}

ActiveCollisions Cursor_touching_border(Cursor* self) {
    ActiveCollisions ac = {
        .up = 0,
        .down = 0,
        .left = 0,
        .right = 0,
    };

    if (self->v_col == self->min_col) {
        ac.left = 1;
    }

    if (self->v_col == self->max_col - 1) {
        ac.right = 1;
    }

    if (self->v_row == self->max_row - 1) {
        ac.down = 1;
    }

    if (self->v_row == self->min_row) {
        ac.up = 1;
    }

    return ac;
}

void Cursor_set_min_row(Cursor* self, const size_t new_min) {
    self->min_row = new_min;
    if (self->v_row < new_min) self->v_row = new_min;
}

void Cursor_set_min_col(Cursor* self, const size_t new_min) {
    self->min_col = new_min;
    if (self->v_col < new_min) self->v_col = new_min;
}

void Cursor_set_max_row(Cursor* self, const size_t new_max) {
    self->max_row = new_max;
    if (self->v_row > new_max) self->v_row = new_max;
}

void Cursor_set_max_col(Cursor* self, const size_t new_max) {
    self->max_col = new_max;
    if (self->v_col > new_max) self->v_col = new_max;
}
