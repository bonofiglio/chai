#ifndef _FS_H
#define _FS_H

#include "screen.h"
#include "utils.h"

typedef struct TextRow {
    size_t size;
    char* chars;
} TextRow;

typedef struct File {
    char* path;
    size_t num_rows;
    TextRow* rows;
} File;

Result File_open(File* self);
void File_free(File* self);
size_t File_get_idx_at_pos(File* self, const size_t col, const size_t row);

#endif
