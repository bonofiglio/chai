#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "canvas.h"
#include "colors.h"
#include "screen.h"
#include "term.h"
#include "utils.h"
#include "xchar.h"

TermState g_term_state;

void draw_text_file(const size_t offset, Canvas* canvas) {
    if (!g_term_state.current_file) return;

    const size_t columns = canvas->width - offset;

    for (size_t fr = 0, cr = 0; fr < canvas->height; fr++) {
        const TextRow* current_row = &g_term_state.current_file->rows[fr];

        if (!current_row->size) {
            cr++;
            continue;
        }

        const size_t lines_to_write = CEIL(current_row->size, columns);

        for (size_t i = 0; i < lines_to_write && cr < canvas->height;
             i++, cr++) {
            size_t r_offset = columns * i;

            size_t size = MIN(current_row->size - r_offset, columns);

            for (size_t j = 0; j < size; j++) {
                char c = current_row->chars[r_offset + j];

                Canvas_set_px(
                    canvas, offset + j, cr,
                    XChar_from_char(c));
            }
        }
    }
}

size_t draw_line_num_rows(Canvas* canvas, const size_t rows) {
    const size_t max_row_width = size_t_width(rows);
    char line_num[max_row_width + 9];

    for (size_t i = 0; i < rows; i++) {
        get_line_number_str(i, max_row_width, line_num);
        size_t line_num_width = strlen(line_num);

        for (size_t j = 0; j < line_num_width; j++) {
            Canvas_set_px(canvas, j, i, XChar_with_color(line_num[j], BLU));
        }
    }

    return max_row_width + 1;
}

void process_key(const char k, const size_t rows, const size_t cols,
                 Cursor* cursor) {
    switch (k) {
        case TO_CTRL('q'):
            exit(0);
            break;
        case 'h': {
            size_t* col = &cursor->col;
            size_t* row = &cursor->row;

            if (*col > cursor->min_col) {
                (*col)--;
            } else if (*row > cursor->min_row) {
                (*row)--;
                *col = cols - 1;
            }
            break;
        }
        case 'l': {
            size_t* col = &cursor->col;
            size_t* row = &cursor->row;

            if (*col < cols - 1) {
                (*col)++;
            } else if (*row < rows - 1) {
                (*row)++;
                *col = cursor->min_col;
            }
            break;
        }
        case 'j': {
            if (cursor->row >= rows - 1) break;

            cursor->row++;
            break;
        }
        case 'k': {
            if (cursor->row <= 0) break;

            cursor->row--;
            break;
        }
    }
}

void main_loop() {
    size_t rows = 0;
    size_t cols = 0;

    const Result result = get_window_size(&rows, &cols);
    if (result == Err) {
        panic("get_window_size");
    }

    Canvas canvas = Canvas_new(cols, rows);
    Cursor cursor = Cursor_new(0, 0);

    // pre-allocate 128kb of vram
    ScreenBuffer scr_buf = ScreenBuffer_new(128000);

    for (;;) {
        ScreenBuffer_reset(&scr_buf);
        ScreenBuffer_write(&scr_buf, ESC "[2J", 4);
        ScreenBuffer_write(&scr_buf, ESC "[H", 3);

        Canvas_clear(&canvas);

        const size_t line_num_width = draw_line_num_rows(&canvas, rows);
        cursor.min_col = line_num_width + 1;

        if (cursor.col < cursor.min_col) cursor.col = cursor.min_col;

        draw_text_file(line_num_width + 1, &canvas);

        CharSizeTuple c_str = Canvas_to_str(&canvas);
        ScreenBuffer_write(&scr_buf, c_str.chars, c_str.size);

        const char* cursor_str = Cursor_to_str(&cursor);
        ScreenBuffer_write(&scr_buf, cursor_str, strlen(cursor_str));

        write(STDIN_FILENO, scr_buf.b, scr_buf.current_index);

        char c = '\0';
        // block thread until an input is received
        while (!read(STDIN_FILENO, &c, 1))
            ;

        process_key(c, rows, cols, &cursor);
    };

    ScreenBuffer_free(&scr_buf);
    Canvas_free(&canvas);
}

int main(const int argc, char *argv[]) {
    File current_file;

    if (argc == 2) {
        printf("argv: %s", argv[1]);

        Result res = File_open(&current_file);
        if (res == Err) panic("File_open");

        g_term_state.current_file = &current_file;
    }

    setup_term();

    main_loop();

    if (argc == 2) {
        File_free(&current_file);
    }

    return 0;
}
