#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#define PATH_MAX 4096  // Define PATH_MAX if not defined
#define MAX_QUEUE_SIZE 1024  // Maximum size of the directory queue

// Structure for directory queue
typedef struct {
    char *directories[MAX_QUEUE_SIZE];
    int front;
    int rear;
} DirectoryQueue;

// Function to initialize the directory queue
void init_queue(DirectoryQueue *queue) {
    queue->front = 0;
    queue->rear = 0;
}

// Function to add a directory to the queue
void enqueue(DirectoryQueue *queue, const char *directory) {
    if (queue->rear < MAX_QUEUE_SIZE) {
        queue->directories[queue->rear++] = strdup(directory);
    }
}

// Function to remove a directory from the queue
char *dequeue(DirectoryQueue *queue) {
    if (queue->front < queue->rear) {
        return queue->directories[queue->front++];
    }
    return NULL;
}

// Function to get the file extension
const char *get_file_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    return (dot && dot != filename) ? dot + 1 : "none";
}

// Function to check read permissions
int has_read_permission(const char *path) {
    return access(path, R_OK) == 0;
}

// Function to list contents of a directory
void list_directory_contents(FILE *file) {
    DirectoryQueue queue;
    init_queue(&queue);
    
    // Enqueue the starting directory (current working directory)
    enqueue(&queue, ".");

    while (queue.front < queue.rear) {
        char *current_dir = dequeue(&queue);
        DIR *dir;
        struct dirent *entry;
        struct stat file_stat;
        char path[PATH_MAX];

        // Open the current directory
        dir = opendir(current_dir);
        if (dir == NULL) {
            if (errno == EACCES) {
                fprintf(file, "%s/ - Permission denied\n", current_dir);
                printf("%s/ - Permission denied\n", current_dir);
            } else {
                perror("Error opening directory");
            }
            free(current_dir);
            continue;
        }

        // Print the current directory heading
        fprintf(file, "\nDirectory: %s\n", current_dir);
        printf("Processing: %s\n", current_dir);

        // Read and write directory entries to the file
        while ((entry = readdir(dir)) != NULL) {
            // Skip "." and ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            // Construct the full path
            snprintf(path, sizeof(path), "%s/%s", current_dir, entry->d_name);

            // Get file statistics
            if (stat(path, &file_stat) == 0) {
                double size_mb = (double)file_stat.st_size / (1024 * 1024);
                const char *extension = S_ISDIR(file_stat.st_mode) ? "N/A" : get_file_extension(entry->d_name);
                const char *read_perm = has_read_permission(path) ? "Readable" : "Not Readable";

                if (S_ISDIR(file_stat.st_mode)) {
                    // If it is a directory, enqueue it for later processing
                    enqueue(&queue, path);
                    fprintf(file, "  %s/ - %.2f MB (Extension: %s, %s)\n", entry->d_name, size_mb, extension, read_perm);
                    printf("  %s/ - Size: %.2f MB (Extension: %s, %s)\n", path, size_mb, extension, read_perm);
                } else {
                    // Print size information for files
                    fprintf(file, "  %s - %.2f MB (Extension: %s, %s)\n", entry->d_name, size_mb, extension, read_perm);
                }
            } else {
                fprintf(file, "  %s - Unable to determine size\n", entry->d_name);
            }
        }

        // Close the directory stream and free the current directory
        closedir(dir);
        free(current_dir);
    }
}
