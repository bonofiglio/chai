#ifndef _UTILS_H
#define _UTILS_H

#define ESC "\x1b"
#define TO_CTRL(key) ((key)&0x1f)

typedef enum Result {Ok = 0, Err = -1} Result;

void panic(const char *fn_name);

#endif
