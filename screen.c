#include "screen.h"

#include <stddef.h>

#include "utils.h"

ScreenBuffer ScreenBuffer_new(const size_t size) {
    ScreenBuffer buf = {
        .size = size,
        .b = malloc(size),
        .current_index = 0
    };

    return buf;
}

void ScreenBuffer_reset(ScreenBuffer *self) { self->current_index = 0; }

void ScreenBuffer_resize(ScreenBuffer *self, size_t new_size) {
    self->b = realloc(self->b, new_size);
    self->size = new_size;
}

void ScreenBuffer_write(ScreenBuffer *self, const char *data,
                        const size_t bytes_written) {
    if (bytes_written + self->current_index > self->size) {
        // resize the screenbuffer if the size is out of bounds
        ScreenBuffer_resize(self,
                            self->size + bytes_written + self->current_index);
    }

    for (size_t i = 0; i < bytes_written; i++) {
        self->b[i + self->current_index] = data[i];
    }
    self->current_index += bytes_written;
}

void ScreenBuffer_clear(ScreenBuffer *self) {
    for (size_t i = 0; i < self->size; i++) {
        self->b[i] = '\0';
    }
}

void ScreenBuffer_free(ScreenBuffer *self) {
    if (!self->b || self->size == 0) return;

    free(self->b);
}
