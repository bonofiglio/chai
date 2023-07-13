#include "term.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "utils.h"

struct termios default_attrs;

Cursor Cursor_new(const size_t min_col, const size_t min_row) {
    Cursor c = {.col = 0, .row = 0, .min_col = min_col, .min_row = min_row, ._str_buf = {'\0'}};

    return c;
}

char *Cursor_to_str(Cursor *self) {
    snprintf(self->_str_buf, sizeof(self->_str_buf), ESC "[%zu;%zuH",
             self->row + 1, self->col + 1);

    return self->_str_buf;
}

Result get_window_size(size_t *rows, size_t *cols) {
    struct winsize ws;

    ertn(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws));

    if (ws.ws_col == 0) return Err;

    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return Ok;
}

void enable_raw_mode() {
    // load the terminal config into term_attrs
    struct termios term_attrs = default_attrs;

    const int input_features_toggle =
        IXON |  // disable software control flow
        ICRNL   // prevent carriage return (ctrl+m) from
                // being translated to newline on input
        ;

    const int output_features_toggle = OPOST  // disable all output processing
        ;

    const int local_features_toggle =
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
    const Result result = tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_attrs);

    if (result == Err) {
        panic("tcsetattr");
    }
}

void init_default_attrs() {
    const Result result = tcgetattr(STDIN_FILENO, &default_attrs);

    if (result == Err) {
        panic("tcgetattr");
    }
}

void restore_term() {
    const Result result = tcsetattr(STDIN_FILENO, TCSANOW, &default_attrs);

    system("tput rmcup");

    if (result == Err) {
        panic("tcsetattr");
    }
}

void setup_term() {
    init_default_attrs();

    system("tput smcup");

    enable_raw_mode();

    atexit(restore_term);
}

Result get_cursor_position(Cursor *cursor) {
    int reading_cols = 0;
    int i_col = 0;
    int i_row = 0;
    char row[3];
    char col[3];

    char c;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return Err;

    for (size_t i = 0; i < 32; i++) {
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

    cursor->row = atoi(row);
    cursor->col = atoi(col);

    return 0;
}
