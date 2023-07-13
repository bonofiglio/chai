#ifndef _SCREEN_H
#define _SCREEN_H
#include <stddef.h>
#include <stdlib.h>

#include "utils.h"

typedef struct ScreenBuffer {
    char* b;
    size_t size;
    size_t current_index;
} ScreenBuffer;

ScreenBuffer ScreenBuffer_new(const size_t size);
void ScreenBuffer_reset(ScreenBuffer* self);
void ScreenBuffer_write(ScreenBuffer* self, const char* data,
                        const size_t bytes);
void ScreenBuffer_clear(ScreenBuffer* self);
void ScreenBuffer_resize(ScreenBuffer* self, size_t new_size);
void ScreenBuffer_free(ScreenBuffer* self);

#endif
