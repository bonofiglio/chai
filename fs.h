#ifndef _FS_H
#define _FS_H

#include "utils.h"
#include "screen.h"

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

#endif
