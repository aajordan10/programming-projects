#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>

#ifndef PATH_MAX
#define PATH_MAX 4096  // Define PATH_MAX if not defined
#endif

// Function prototypes
void get_current_directory(char *buffer);
void list_directory_contents(FILE *file);

int main() {
    char cwd[PATH_MAX];
    FILE *file;

    // Start timing
    clock_t start_time = clock();

    // Open the file for writing
    file = fopen("overview.txt", "w");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Get the current working directory
    get_current_directory(cwd);
    fprintf(file, "Current working directory: %s\n", cwd);

    // Change to the highest possible directory (root directory)
    if (chdir("/") != 0) {
        perror("chdir() to root directory error");
        fclose(file);
        return 1;
    }

    // List the directory contents of the root directory
    fprintf(file, "Directory contents of the root directory:\n");
    list_directory_contents(file);

    // End timing
    clock_t end_time = clock();
    double time_elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Print time elapsed
    fprintf(file, "\nTime elapsed: %.2f seconds\n", time_elapsed);
    printf("Time elapsed: %.2f seconds\n", time_elapsed);

    // Clean up
    fclose(file);

    printf("Overview has been written to overview.txt\n");

    return 0;
}
