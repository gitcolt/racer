#include "debug.h"
#include "ansi_colors.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void err_exit(const char *msg,
              const char *filename,
              const char *funcname,
              int line,
              ...) {
    va_list args;
    va_start(args, line);

    fprintf(stderr, RED_TXT "[%s:%s:%d]: ", filename, funcname, line);
    vfprintf(stderr, msg, args);
    fprintf(stderr, RST_COL);
    fflush(stderr);

    va_end(args);

    exit(EXIT_FAILURE);
}
