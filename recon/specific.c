#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define PATH_MAX 4096

// Function to check if the given path is a valid directory
int is_valid_directory(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return 0; // Unable to get the status of the path
    }
    return S_ISDIR(path_stat.st_mode); // Return true if it's a directory
}

// Function to get the size of a file
long long get_file_size(const char *path) {
    struct stat file_stat;
    if (stat(path, &file_stat) == 0) {
        return file_stat.st_size;
    }
    return -1; // Error in getting file size
}

// Function to list contents of a directory and traverse subdirectories
void list_directory_contents(FILE *output_file, const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    fprintf(output_file, "Contents of %s:\n", path);
    printf("Processing: %s\n", path);
    
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            // Construct the full path for the entry
            char full_path[PATH_MAX];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

            // Get the size of the entry
            long long size = get_file_size(full_path);
            if (size != -1) {
                fprintf(output_file, "  %s - Size: %lld bytes\n", entry->d_name, size);
            } else {
                fprintf(output_file, "  %s - Size: Unknown\n", entry->d_name);
            }

            // If the entry is a directory, recurse into it
            if (is_valid_directory(full_path)) {
                list_directory_contents(output_file, full_path);
            }
        }
    }

    closedir(dir);
}

int main() {
    char input_path[PATH_MAX];

    // Open the output file for writing
    FILE *output_file = fopen("specific.txt", "w");
    if (output_file == NULL) {
        perror("Error opening output file");
        return EXIT_FAILURE;
    }

    // Get user input for the directory path
    printf("Enter a directory path: ");
    if (fgets(input_path, sizeof(input_path), stdin) != NULL) {
        // Remove the newline character from the input
        input_path[strcspn(input_path, "\n")] = '\0';

        // Validate the directory
        if (is_valid_directory(input_path)) {
            list_directory_contents(output_file, input_path);
        } else {
            fprintf(output_file, "Invalid input: '%s' is not a valid directory.\n", input_path);
            printf("Invalid input: '%s' is not a valid directory.\n", input_path);
        }
    } else {
        fprintf(output_file, "Error reading input.\n");
        printf("Error reading input.\n");
    }

    fclose(output_file); // Close the output file
    return 0;
}
