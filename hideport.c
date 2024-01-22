#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

static int (*original_open)(const char *pathname, int flags, ...) = NULL;
static int redirected_fd = -1;

int open(const char *pathname, int flags, ...) {
  // Load the original open function if not loaded
  if (!original_open) {
    original_open = dlsym(RTLD_NEXT, "open");
    if (!original_open) {
      fprintf(stderr, "Error: Unable to load original open function\n");
      return -1;
    }
  }

  // Check if the file being opened is /proc/net/tcp
  if (strcmp(pathname, "/proc/net/tcp") == 0) {
    // If not already redirected, open the new file
    if (redirected_fd == -1) {
      redirected_fd = original_open("/dev/null", O_RDONLY);
      if (redirected_fd == -1) {
        fprintf(stderr, "Error: Unable to open /dev/null\n");
        return -1;
      }
    }

    // Return the redirected file descriptor
    return redirected_fd;
  } else {
    // Call the original open function for other files
    return original_open(pathname, flags);
  }
}
