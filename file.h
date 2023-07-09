#include "utils.h"

#ifndef _FILE_H
#define _FILE_H

typedef struct Row {
    size_t size;
    char* chars;
} Row;

typedef struct File {
    char* path;
    size_t num_rows;
    Row* rows;
} File;

Result File_open(File* self);
void File_free(File* self);

#endif
