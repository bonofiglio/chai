#include <stddef.h>
#include <string.h>
#include <unistd.h>

#include "cursor.h"
#include "term.h"
#include "utils.h"
#include "xchar.h"

void draw_text_file(TermState* state, const size_t h_pos,
                    const size_t max_height) {
    if (!state->current_file) return;

    const size_t columns = state->canvas.width - h_pos;

    for (size_t fr = state->scroll_pos, cr = 0;
         fr < max_height + state->scroll_pos; fr++) {
        const TextRow* current_row = &state->current_file->rows[fr];

        if (!current_row->size) {
            cr++;
            continue;
        }

        const size_t lines_to_write = CEIL(current_row->size, columns);

        if (lines_to_write + cr > max_height) break;

        for (size_t i = 0; i < lines_to_write; i++, cr++) {
            size_t r_offset = columns * i;

            size_t size = MIN(current_row->size - r_offset, columns);

            for (size_t j = 0; j < size; j++) {
                char c = current_row->chars[r_offset + j];

                Canvas_set_px(&state->canvas, h_pos + j, cr,
                              XChar_from_char(c));
            }
        }
    }
}

size_t draw_line_num_rows(TermState* state, const size_t rows) {
    const size_t max_row_width = size_t_width(state->current_file->num_rows);
    const size_t line_num_width = max_row_width + 2;
    char line_num[max_row_width + 9];

    size_t prev_line_num = 0;
    size_t line_num_at = 0;

    size_t lines_acc = 0;
    for (size_t i = 0; i < state->scroll_pos; i++) {
        lines_acc += TermState_get_text_row_height(state, i, line_num_width);
    }

    for (size_t i = 0; i < rows; i++) {
        const size_t row_height = TermState_get_text_row_height(
            state, i + state->scroll_pos, line_num_width);

        if (row_height + i > rows) break;

        line_num_at =
            TermState_get_text_row_at(state, i + lines_acc, line_num_width);

        if (line_num_at == prev_line_num) {
            prev_line_num = line_num_at;

            continue;
        }

        get_line_number_str(line_num_at, max_row_width, line_num);

        for (size_t j = 0; j < line_num_width; j++) {
            const XChar xc = XChar_with_color(line_num[j], BLU);
            Canvas_set_px(&state->canvas, j, i, xc);
        }

        prev_line_num = line_num_at;
    }

    return max_row_width + 2;
}

void process_key(TermState* state, const char k, const size_t max_height) {
    ActiveCollisions ac = Cursor_touching_border(&state->cursor);

    switch (k) {
        case TO_CTRL('c'):
            exit(0);
            break;
        case 'h': {
            Cursor_move(&state->cursor, Left);
            if (ac.left && ac.up) {
                TermState_scoll(state, Up, max_height);
            }
            break;
        }
        case 'l': {
            Cursor_move(&state->cursor, Right);
            if (ac.right && ac.down) {
                TermState_scoll(state, Down, max_height);
            }
            break;
        }
        case 'j': {
            Cursor_move(&state->cursor, Down);
            if (ac.down) {
                TermState_scoll(state, Down, max_height);
            }
            break;
        }
        case 'k': {
            Cursor_move(&state->cursor, Up);
            if (ac.up) {
                TermState_scoll(state, Up, max_height);
            }
            break;
        }
    }
}

void main_loop(TermState* state) {
    // pre-allocate 128kb of vram
    ScreenBuffer scr_buf = ScreenBuffer_new(128000);

    for (;;) {
        ScreenBuffer_reset(&scr_buf);
        ScreenBuffer_write(&scr_buf, ESC "[2J", 4);
        ScreenBuffer_write(&scr_buf, ESC "[H", 3);

        Canvas_clear(&state->canvas);
        const size_t line_num_width =
            draw_line_num_rows(state, state->canvas.height - 1);

        Cursor_set_min_col(&state->cursor, line_num_width);
        Cursor_set_max_row(&state->cursor, state->canvas.height - 1);
        draw_text_file(state, line_num_width, state->canvas.height - 1);

        CharSizeTuple c_str = Canvas_to_str(&state->canvas);
        ScreenBuffer_write(&scr_buf, c_str.chars, c_str.size);

        const char* cursor_str = Cursor_to_str(&state->cursor);
        ScreenBuffer_write(&scr_buf, cursor_str, strlen(cursor_str));

        write(STDIN_FILENO, scr_buf.b, scr_buf.current_index);

        char c = '\0';
        // block thread until an input is received
        while (!read(STDIN_FILENO, &c, 1))
            ;

        process_key(state, c, state->canvas.height - 1);
    };

    ScreenBuffer_free(&scr_buf);
}

int main(const int argc, char* argv[]) {
    setup_term();

    size_t rows = 0;
    size_t cols = 0;

    if (get_window_size(&rows, &cols) == Err) {
        panic("get_window_size");
    }

    TermState state =
        TermState_new(Canvas_new(rows, cols), Cursor_new(0, 0, rows, cols));

    File current_file;

    if (argc == 2) {
        current_file.path = argv[1];

        Result res = File_open(&current_file);
        if (res == Err) panic("File_open");

        state.current_file = &current_file;
    }

    main_loop(&state);

    TermState_free(&state);

    return 0;
}
