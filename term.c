#include "term.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "utils.h"

struct termios default_attrs;

void ScreenBuffer_resize(ScreenBuffer *self, unsigned int new_size) {
    // extra room for escape sequences
    new_size += ESC_SEQ_SPACE;

    self->b = realloc(self->b, new_size * sizeof(char));
    self->size = new_size;
}

void ScreenBuffer_write(ScreenBuffer *self, const char *data,
                           const unsigned int bytes_written) {
    if (bytes_written + self->current_index > self->size) {
        // resize the screenbuffer if the size is out of bounds
        ScreenBuffer_resize(
            self, self->size + bytes_written + self->current_index);
    }

    for (unsigned int i = 0; i < bytes_written; i++) {
        self->b[i + self->current_index] = data[i];
    }
    self->current_index += bytes_written;
}

void ScreenBuffer_clear(ScreenBuffer *self) {
    for (unsigned int i = 0; i < self->size; i++) {
        self->b[i] = '\0';
    }

    self->current_index = 0;
}

void ScreenBuffer_free(ScreenBuffer *self) {
    if (!self->b || self->size == 0) return;

    free(self->b);
}

Result get_window_size(int *rows, int *cols) {
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == Err || ws.ws_col == 0) {
        return Err;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return Ok;
    }
}

void enable_raw_mode() {
    // load the terminal config into term_attrs
    struct termios term_attrs = default_attrs;

    const int input_features_toggle = IXON |  // disable software control flow
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

    cursor->row = atoi(row);
    cursor->col = atoi(col);

    return 0;
}
