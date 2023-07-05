#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "term.h"
#include "utils.h"

TermState g_term_state;

char cursor_movement_buf[32];

void draw() {
    ScreenBuffer_clear(&g_term_state.screen_buffer);
    ScreenBuffer_write(&g_term_state.screen_buffer, ESC "[H", 3);

    for (int i = 0; i < g_term_state.screen.rows; i++) {
        if (i != g_term_state.screen.rows - 1) {
            ScreenBuffer_write(&g_term_state.screen_buffer,
                                   "~\r\n" ESC "[K", 6);

            continue;
        }

        ScreenBuffer_write(&g_term_state.screen_buffer, "~" ESC "[K", 4);
    }

    snprintf(cursor_movement_buf, sizeof(cursor_movement_buf), ESC "[%d;%dH", g_term_state.cursor.row + 1,
             g_term_state.cursor.col + 1);

    ScreenBuffer_write(&g_term_state.screen_buffer, cursor_movement_buf, strlen(cursor_movement_buf));
}

void process_key(const char k) {
    switch (k) {
        case TO_CTRL('q'):
            exit(0);
            break;
        case 'h': {
            int* col = &g_term_state.cursor.col;
            int* row = &g_term_state.cursor.row;

            if (*col > 0) {
                (*col)--;
            } else if (*row > 0) {
                (*row)--;
                *col = g_term_state.screen.columns;
            }
            break;
        }
        case 'l': {
            int* col = &g_term_state.cursor.col;
            int* row = &g_term_state.cursor.row;

            if (*col < g_term_state.screen.columns) {
                (*col)++;
            } else if (*row < g_term_state.screen.rows) {
                (*row)++;
                *col = 0;
            }
            break;
        }
        case 'j': {
            if (g_term_state.cursor.row >= g_term_state.screen.rows) break;

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

    const Result result = get_window_size(&g_term_state.screen.rows,
                                     &g_term_state.screen.columns);

    if (result == Err) {
        panic("get_window_size");
    }

    ScreenBuffer_resize(&g_term_state.screen_buffer, g_term_state.screen.rows * g_term_state.screen.columns);

    for (;;) {
        draw();

        write(STDIN_FILENO, g_term_state.screen_buffer.b,
              g_term_state.screen_buffer.size);

        c = '\0';
        read(STDIN_FILENO, &c, 1);

        process_key(c);
    };

    ScreenBuffer_free(&g_term_state.screen_buffer);
}

int main() {
    setup_term();

    main_loop();

    return 0;
}
