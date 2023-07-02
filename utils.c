#include <stdio.h>
#include <stdlib.h>

#include "term.h"

void panic(const char *fn_name) {
    restore_term();
    perror(fn_name);
    exit(1);
}
