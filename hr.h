#ifndef HOTRLD_H
#define HOTRLD_H

void _hr_register_load_hook();
void _hr_register_close_hook();
void _hr_register_unload_hook();
void _hr_register_update_hook();
void _hr_register_hooks();

extern void (*_hr_fptr_update)(void *ctx);
extern void (*_hr_fptr_load)(void *ctx);
extern void (*_hr_fptr_unload)(void *ctx);
extern void (*_hr_fptr_close)(void *ctx);

#define HR_HOOK_UPDATE(fptr) \
void _hr_register_hooks() { \
    _hr_register_unload_hook(); \
    _hr_register_load_hook(); \
    _hr_register_close_hook(); \
    _hr_register_update_hook(); \
} \
void _hr_register_update_hook() { \
    _hr_fptr_update = fptr; \
}

#define HR_HOOK_LOAD(fptr) \
void _hr_register_load_hook() { \
    _hr_fptr_load = fptr; \
}

#define HR_HOOK_UNLOAD(fptr) \
void _hr_register_unload_hook() { \
    _hr_fptr_unload = fptr; \
}

#define HR_HOOK_CLOSE(fptr) \
void _hr_register_close_hook() { \
    _hr_fptr_close = fptr; \
}

#endif // end HOTRLD_H

/*
 * =============================================================
 */

#ifdef HR_DRIVER

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <limits.h>
#include <errno.h>
#include <sys/inotify.h>
#include <string.h>
#include <stdbool.h>

const char *build_cmd_prefix = "make -B ";

static bool first_time = true;
static void *lib_handle;
static const char *shared_lib_str;
static int inotify_fd;
#define INOTIFY_EVENT_BUF_MAX (sizeof(struct inotify_event) + NAME_MAX + 1)
static char inotify_event_buf[INOTIFY_EVENT_BUF_MAX];

void hr_init(const char *watch_dir, const char *make_target);
void hr_update();
void hr_close();

void (*_hr_fptr_update)(void *ctx);
void (*_hr_fptr_load)(void *ctx);
void (*_hr_fptr_unload)(void *ctx);
void (*_hr_fptr_close)(void *ctx);
void (*_hr_fptr_register_hooks)();

static void *_hr_ctx = NULL;

#define HR_CTX(ctx) \
    _hr_ctx = &ctx;

void hr_watch_init(const char *watch_dir) {
    inotify_fd = inotify_init();
    if (inotify_fd < 0) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }
    int flags = fcntl(inotify_fd, F_GETFL);
    fcntl(inotify_fd, F_SETFL, flags | O_NONBLOCK);

    int wd = inotify_add_watch(inotify_fd, watch_dir, IN_MODIFY);
    if (wd < 0) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }
    int wd2 = inotify_add_watch(inotify_fd, "overworld", IN_MODIFY);
    if (wd2 < 0) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }
    int wd3 = inotify_add_watch(inotify_fd, "animation", IN_MODIFY);
    if (wd3 < 0) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }
    int wd4 = inotify_add_watch(inotify_fd, "dialog", IN_MODIFY);
    if (wd4 < 0) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }
    int wd5 = inotify_add_watch(inotify_fd, "race", IN_MODIFY);
    if (wd5 < 0) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }
}

bool hr_check_modified() {
    int len = read(inotify_fd, inotify_event_buf, INOTIFY_EVENT_BUF_MAX);
    if (len == -1 && errno == EAGAIN)
        return false;
    const char *needles[] = { ".c", ".h" };
    int i = 0;
    while (i < len) {
        struct inotify_event *ev = (struct inotify_event *)&inotify_event_buf[i];
        for (int j = 0; j < sizeof(needles)/sizeof(*needles); ++j) {
            if (strstr(ev->name, needles[j]))
                return true;
        }
        i += sizeof(struct inotify_event) + ev->len;
    }
    return false;
}

void hr_close() {
    if (_hr_fptr_close)
        _hr_fptr_close(_hr_ctx);
    dlclose(lib_handle);
    // system("rm libmodule.so");
}

void hr_reload() {
    if (first_time)
        first_time = false;
    else {
        if (_hr_fptr_unload)
            _hr_fptr_unload(_hr_ctx);
        hr_close(_hr_ctx);
    }

    char sys_cmd[99];
    strcpy(sys_cmd, build_cmd_prefix);
    strcat(sys_cmd, shared_lib_str);
    system(sys_cmd);
    // https://unix.stackexchange.com/questions/607652/why-the-dynamic-linker-couldnt-resolve-reference-when-a-shared-library-has-a-de
    lib_handle = dlopen(shared_lib_str, RTLD_NOW | RTLD_DEEPBIND);
    /*
     * Setting the RTLD_DEEPBIND flag is the only way I've been able to get the linker
     * to respect changes to the dlopen'ed library when using a source file in both the
     * driver code and the module. I imagine there's a better way that doesn't require
     * such a new version of glibc (2.3.4 2004!)
     */
    if (!lib_handle) {
        fprintf(stderr, "dlopen: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    _hr_fptr_register_hooks = dlsym(lib_handle, "_hr_register_hooks");
    if (!_hr_fptr_register_hooks) 
        fprintf(stderr, "dlsym: %s\n", dlerror());
    _hr_fptr_register_hooks();

    if (_hr_fptr_load)
        _hr_fptr_load(_hr_ctx);
}

void hr_update() {
    if (hr_check_modified())
        hr_reload();
    _hr_fptr_update(_hr_ctx);
}

void hr_init(const char *watch_dir, const char *shared_lib) {
    shared_lib_str = shared_lib;
    hr_reload(_hr_ctx);
    hr_watch_init(watch_dir);
}

#endif // end implementation
