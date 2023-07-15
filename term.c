#include "term.h"

#include <stddef.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

struct termios default_attrs;

TermState TermState_new(Canvas canvas, Cursor cursor) {
    TermState ts = {.canvas = canvas,
                    .cursor = cursor,
                    .scroll_pos = 0,
                    .current_file = NULL};

    return ts;
}

void TermState_scoll(TermState *self, enum Directions dir,
                     const size_t max_height) {
    switch (dir) {
        case Up:
            if (self->scroll_pos > 0) {
                self->scroll_pos--;
            }
            break;
        case Down:
            if (max_height + self->scroll_pos < self->current_file->num_rows) {
                self->scroll_pos++;
            }
            break;
        default:
            break;
    }
}

size_t TermState_get_text_row_height(TermState *self, const size_t idx,
                                     const size_t padding) {
    const TextRow *current_row = &self->current_file->rows[idx];

    if (current_row->size == 0) return 1;

    const size_t row_lines =
        CEIL(current_row->size, self->canvas.width - padding);

    return row_lines;
}

size_t TermState_get_text_row_at(TermState *self, const size_t y,
                                 const size_t padding) {
    size_t ri = 0;

    for (size_t i = 0; i <= y && ri < self->current_file->num_rows; ri++) {
        const TextRow *current_row = &self->current_file->rows[ri];

        const size_t row_height =
            TermState_get_text_row_height(self, ri, padding);

        i += row_height;
    }

    return ri;
}

void TermState_free(TermState *self) {
    Canvas_free(&self->canvas);

    if (self->current_file) {
        File_free(self->current_file);
    }
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
