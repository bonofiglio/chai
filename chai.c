#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "term.h"
#include "utils.h"

TermState g_term_state;

char buf[32];

void draw() {
    mut_clear_screenbuffer(&g_term_state.screen_buffer);
    mut_write_screenbuffer(&g_term_state.screen_buffer, ESC "[H", 3);

    for (int i = 0; i < g_term_state.screen.rows; i++) {
        if (i != g_term_state.screen.rows - 1) {
            mut_write_screenbuffer(&g_term_state.screen_buffer,
                                   "~\r\n" ESC "[K", 6);

            continue;
        }

        mut_write_screenbuffer(&g_term_state.screen_buffer, "~" ESC "[K", 4);
    }

    snprintf(buf, sizeof(buf), ESC "[%d;%dH", g_term_state.cursor.row + 1,
             g_term_state.cursor.col + 1);

    mut_write_screenbuffer(&g_term_state.screen_buffer, buf, strlen(buf));
}

void process_key(char k) {
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

    int result = mut_get_window_size(&g_term_state.screen.rows,
                                     &g_term_state.screen.columns);

    if (result == -1) {
        panic("mut_get_window_size");
    }

    mut_resize_screenbuffer(&g_term_state.screen_buffer, g_term_state.screen.rows * g_term_state.screen.columns);

    for (;;) {
        draw();

        write(STDIN_FILENO, g_term_state.screen_buffer.b,
              g_term_state.screen_buffer.size);

        c = '\0';
        read(STDIN_FILENO, &c, 1);

        process_key(c);
    };

    mut_free_screenbuffer(&g_term_state.screen_buffer);
}

int main() {
    setup_term();

    main_loop();

    return 0;
}
