#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "colors.h"
#include "term.h"

size_t size_t_width(const size_t num) {
    if (num > ZU_MAX_NIN) {
        return 10;
    } else if (num > ZU_MAX_EIG) {
        return 9;
    } else if (num > ZU_MAX_SEV) {
        return 8;
    } else if (num > ZU_MAX_SIX) {
        return 7;
    } else if (num > ZU_MAX_FIV) {
        return 6;
    } else if (num > ZU_MAX_FOU) {
        return 5;
    } else if (num > ZU_MAX_THR) {
        return 4;
    } else if (num > ZU_MAX_TWO) {
        return 3;
    } else if (num > ZU_MAX_ONE) {
        return 2;
    } else {
        return 1;
    }
}

void panic(const char* fn_name) {
    restore_term();
    perror(fn_name);
    exit(1);
}

void generate_empty_string(char* out, size_t len) {
    for (size_t i = 0; i < len; i++) {
        out[i] = ' ';
    }
}

char* get_line_number_str(const size_t row, const size_t max_row) {
    const size_t max_row_len = size_t_width(max_row);
    const size_t row_len = size_t_width(row);
    const size_t left_pad_len = MAX(max_row_len - row_len, 0);

    char left_pad[MAX(left_pad_len, 1)];

    generate_empty_string(&left_pad[0], sizeof(left_pad));

    if (left_pad_len == 0) left_pad[0] = '\0';

    char* line_num =
        format(sizeof(left_pad) + row_len + 3, " %s%zu ", left_pad, row);

    return line_num;
}
