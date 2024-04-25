#ifndef DEBUG_H
#define DEBUG_H

void err_exit(const char *msg,
              const char *filename,
              const char *funcname,
              int line,
              ...);

// ', ##' elides the comma if __VA_ARGS__ is empty
// See https://gcc.gnu.org/onlinedocs/gcc-4.6.4/gcc/Variadic-Macros.html
#define ERR_EXIT(msg, ...) \
    err_exit(msg, __FILE__, __func__, __LINE__, ## __VA_ARGS__)


#endif
