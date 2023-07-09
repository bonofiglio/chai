#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "colors.h"
#include "term.h"
#include "utils.h"

TermState g_term_state;

char cursor_movement_buf[10];

int draw_file_text(size_t* fr, size_t* sr) {
    char* line_num = get_line_number_str(*sr, g_term_state.w_screen.rows);
    size_t line_num_width = strlen(line_num); 


    ScreenBuffer_write(&g_term_state.screen_buffer, BLU, 5);
    ScreenBuffer_write(&g_term_state.screen_buffer, line_num, line_num_width);
    ScreenBuffer_write(&g_term_state.screen_buffer, RESET, 4);
    free(line_num);

    const size_t columns = g_term_state.w_screen.columns - line_num_width;

    const Row* current_row = &g_term_state.current_file->rows[*fr];

    if (!current_row->size) {
        ScreenBuffer_write(&g_term_state.screen_buffer, "\r\n", 2);
        return 0;
    }

    const int lines_to_write = CEIL(current_row->size, columns);

    for (int i = 0; i < lines_to_write; i++, (*sr)++) {
        size_t offset = columns * i;

        size_t size = MIN(current_row->size - offset, columns);

        ScreenBuffer_write(&g_term_state.screen_buffer,
                           &current_row->chars[columns * i], size);

        // if on the last line, don't write a line break to avoid overflowing
        // the screen
        if ((*sr) == g_term_state.w_screen.rows - 1) {
            return 1;
        }

        ScreenBuffer_write(&g_term_state.screen_buffer, "\r\n", 2);
    }

    (*sr)--;

    return 0;
}

void draw() {
    ScreenBuffer_reset(&g_term_state.screen_buffer);
    ScreenBuffer_write(&g_term_state.screen_buffer, ESC "[2J", 4);
    ScreenBuffer_write(&g_term_state.screen_buffer, ESC "[H", 3);

    for (size_t sr = 0, fr = 0; sr < g_term_state.w_screen.rows; sr++, fr++) {
        if (g_term_state.current_file &&
            fr < g_term_state.current_file->num_rows) {
            if (draw_file_text(&fr, &sr) == 1) break;

            continue;
        }

        if (sr != g_term_state.w_screen.rows - 1) {
            ScreenBuffer_write(&g_term_state.screen_buffer, BLU "~\r\n" RESET,
                               12);

            continue;
        }

        if (g_term_state.current_file) {
            ScreenBuffer_write(&g_term_state.screen_buffer,
                               g_term_state.current_file->path,
                               strlen(g_term_state.current_file->path));
        }
    }

    int len = snprintf(cursor_movement_buf, sizeof(cursor_movement_buf),
                       ESC "[%zu;%zuH", g_term_state.cursor.row + 1,
                       g_term_state.cursor.col + 1);

    ScreenBuffer_write(&g_term_state.screen_buffer, cursor_movement_buf, len);
}

void process_key(const char k) {
    switch (k) {
        case TO_CTRL('q'):
            exit(0);
            break;
        case 'h': {
            size_t* col = &g_term_state.cursor.col;
            size_t* row = &g_term_state.cursor.row;

            if (*col > 0) {
                (*col)--;
            } else if (*row > 0) {
                (*row)--;
                *col = g_term_state.w_screen.columns - 1;
            }
            break;
        }
        case 'l': {
            size_t* col = &g_term_state.cursor.col;
            size_t* row = &g_term_state.cursor.row;

            if (*col < g_term_state.w_screen.columns - 1) {
                (*col)++;
            } else if (*row < g_term_state.w_screen.rows - 1) {
                (*row)++;
                *col = 0;
            }
            break;
        }
        case 'j': {
            if (g_term_state.cursor.row >= g_term_state.w_screen.rows - 1)
                break;

            g_term_state.cursor.row++;
            break;
        }
        case 'k': {
            if (g_term_state.cursor.row <= 0) break;

            g_term_state.cursor.row--;
            break;
        }
    }
}

void main_loop() {
    char c;

    write(STDIN_FILENO, SCREEN_REFRESH, strlen(SCREEN_REFRESH));

    const Result result = get_window_size(&g_term_state.w_screen.rows,
                                          &g_term_state.w_screen.columns);

    if (result == Err) {
        panic("get_window_size");
    }

    // pre-allocate 128kb of vram
    ScreenBuffer_resize(&g_term_state.screen_buffer, 128000);

    for (;;) {
        draw();

        write(STDIN_FILENO, g_term_state.screen_buffer.b,
              g_term_state.screen_buffer.current_index);

        c = '\0';
        // block thread until an input is received
        while (!read(STDIN_FILENO, &c, 1))
            ;

        process_key(c);
    };
}

int main() {
    File current_file;

    current_file.path = "/Users/danielbonofiglio/test.txt";

    Result res = File_open(&current_file);

    if (res == Err) panic("File_open");

    g_term_state.current_file = &current_file;

    setup_term();

    main_loop();

    ScreenBuffer_free(&g_term_state.screen_buffer);
    File_free(&current_file);

    return 0;
}
