#include "fs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

Result File_open(File* self) {
    self->num_rows = 0;
    self->rows = NULL;

    FILE* fp = fopen(self->path, "r");

    if (!fp) return Err;

    char* line = NULL;
    size_t linecap = 0;
    long linelen;

    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        while (linelen > 0 &&
               (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
            linelen--;

        // allocate space for a new row
        size_t new_size = sizeof(TextRow) * (self->num_rows + 1);

        self->rows = realloc(self->rows, new_size);

        self->rows[self->num_rows].size = linelen;
        self->rows[self->num_rows].chars = malloc(linelen + 1);

        memcpy(self->rows[self->num_rows].chars, line, linelen);

        self->rows[self->num_rows].chars[linelen] = '\0';

        self->num_rows++;
    }

    free(line);
    ertn(fclose(fp));

    return Ok;
}

void File_free(File* self) {
    for (size_t i = 0; i < self->num_rows; i++) {
        free(self->rows[i].chars);
    }

    free(self->rows);
    free(self->path);
}

