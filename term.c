#include "term.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "utils.h"

struct termios default_attrs;

void mut_resize_screenbuffer(ScreenBuffer *mut_self, unsigned int new_size) {
    // extra room for escape sequences
    new_size += ESC_SEQ_SPACE;

    mut_self->b = realloc(mut_self->b, new_size * sizeof(char));
    mut_self->size = new_size;
}

int mut_write_screenbuffer(ScreenBuffer *mut_self, char *data,
                           unsigned int bytes_written) {
    if (bytes_written + mut_self->current_index > mut_self->size) {
        // resize the screenbuffer if the size is out of bounds
        mut_resize_screenbuffer(
            mut_self, mut_self->size + bytes_written + mut_self->current_index);
    }

    for (unsigned int i = 0; i < bytes_written; i++) {
        mut_self->b[i + mut_self->current_index] = data[i];
    }
    mut_self->current_index += bytes_written;

    return 0;
}

void mut_clear_screenbuffer(ScreenBuffer *mut_self) {
    for (unsigned int i = 0; i < mut_self->size; i++) {
        mut_self->b[i] = '\0';
    }

    mut_self->current_index = 0;
}

void mut_free_screenbuffer(ScreenBuffer *mut_self) {
    if (!mut_self->b || mut_self->size == 0) return;

    free(mut_self->b);
}

int mut_get_window_size(int *mut_rows, int *mut_cols) {
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    } else {
        *mut_cols = ws.ws_col;
        *mut_rows = ws.ws_row;
        return 0;
    }
}

void enable_raw_mode() {
    // load the terminal config into term_attrs
    struct termios term_attrs = default_attrs;

    int input_features_toggle = IXON |  // disable software control flow
                                ICRNL   // prevent carriage return (ctrl+m) from
                                        // being translated to newline on input
        ;

    int output_features_toggle = OPOST  // disable all output processing
        ;

    int local_features_toggle =
        ECHO |    // disable printing of the keys when they are pressed
        ICANON |  // read byte by byte instead of lines
        ISIG |    // disable INTR, QUIT, SUSP, or DSUSP signals
        IEXTEN |  // disable ctrl+v. Also disables ctrl+o (MacOS only)
        BRKINT |  // probably not needed in this day and age, but it's there for
                  // peace of mind
        INPCK |   // same as above
        ISTRIP    // same as above
        ;

    // toggle the different flag bits to turn off the features
    term_attrs.c_lflag &= ~local_features_toggle;
    term_attrs.c_oflag &= ~output_features_toggle;
    term_attrs.c_iflag &= ~input_features_toggle;

    // set character size to 8 bytes (probably the default already)
    term_attrs.c_cflag |= CS8;

    // set the minimum requirement of chars for the read function to return to 0
    term_attrs.c_cc[VMIN] = 0;

    // set the wait time of the read function to 1/10 seconds (100ms)
    term_attrs.c_cc[VTIME] = 1;

    // update the terminal config
    int result = tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_attrs);

    if (result == -1) {
        panic("tcsetattr");
    }
}

void init_default_attrs() {
    int result = tcgetattr(STDIN_FILENO, &default_attrs);

    if (result == -1) {
        panic("tcgetattr");
    }
}

void restore_term() {
    int result = tcsetattr(STDIN_FILENO, TCSANOW, &default_attrs);

    system("tput rmcup");

    if (result == -1) {
        panic("tcsetattr");
    }
}

void setup_term() {
    init_default_attrs();

    system("tput smcup");

    enable_raw_mode();

    atexit(restore_term);
}

int mut_get_cursor_position(Cursor *mut_cursor) {
    int reading_cols = 0;
    int i_col = 0;
    int i_row = 0;
    char row[3];
    char col[3];

    char c;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

    for (unsigned int i = 0; i < 32; i++) {
        if (read(STDIN_FILENO, &c, 1) != 1) break;
        if (c == 'R') break;

        // skip the first two characters
        if (i < 2) {
            continue;
        }

        if (c == ';') {
            reading_cols = 1;
            continue;
        }

        if (reading_cols) {
            col[i_col] = c;
            i_col++;
        } else {
            row[i_row] = c;
            i_row++;
        }
    }

    col[i_col] = '\0';
    row[i_row] = '\0';

    mut_cursor->row = atoi(row);
    mut_cursor->col = atoi(col);

    return 0;
}
