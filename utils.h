#ifndef _UTILS_H
#define _UTILS_H

#include <stdlib.h>
#define ESC "\x1b"
#define TO_CTRL(key) ((key)&0x1f)
#define CEIL(A, B) (A + (B - 1)) / B
#define MIN(A, B) (A < B ? A : B)
#define MAX(A, B) (A > B ? A : B)

#define ZU_MAX_NIN 999999999
#define ZU_MAX_EIG 99999999
#define ZU_MAX_SEV 9999999
#define ZU_MAX_SIX 999999
#define ZU_MAX_FIV 99999
#define ZU_MAX_FOU 9999
#define ZU_MAX_THR 999
#define ZU_MAX_TWO 99
#define ZU_MAX_ONE 9

// formats a string and returns a pointer to the formatted string
#define format(reserved_bytes, format_string, ...)                     \
    ({                                                                 \
        char* out_buf = malloc(reserved_bytes);                        \
        snprintf(out_buf, reserved_bytes, format_string, __VA_ARGS__); \
        out_buf;                                                       \
    })

//  returns Err (-1) when the response is an error (-1)
#define ertn(res)     \
    if (res == Err) { \
        return Err;   \
    }

typedef enum Result { Ok = 0, Err = -1 } Result;

size_t size_t_width(const size_t num);
void panic(const char* fn_name);
void generate_empty_string(char* out, size_t len);
char* get_line_number_str(const size_t row, const size_t max_row);

#endif
