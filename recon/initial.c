#include <stdio.h>
#include <unistd.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096  // Define a reasonable maximum path length if PATH_MAX is not defined
#endif

void get_current_directory(char *buffer) {
    // Get the current working directory
    if (getcwd(buffer, PATH_MAX) == NULL) {
        perror("getcwd() error");
    }
}