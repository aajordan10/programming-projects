#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

void flip_bits_in_file(const char *input_path, const char *output_path) {
    FILE *in_fp = fopen(input_path, "rb");
    if (in_fp == NULL) {
        perror("Error opening input file");
        return;
    }

    FILE *out_fp = fopen(output_path, "wb");
    if (out_fp == NULL) {
        perror("Error opening output file");
        fclose(in_fp);
        return;
    }

    int byte;
    while ((byte = fgetc(in_fp)) != EOF) {
        byte = ~byte;  // Flip the bits
        fputc(byte, out_fp);
    }

    fclose(in_fp);
    fclose(out_fp);
}

int check_files_in_directory(const char *dir_path) {
    struct dirent *entry;
    DIR *dp = opendir(dir_path);

    if (dp == NULL) {
        perror("Error opening directory");
        return 0;
    }

    int file_count = 0;
    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_REG) {  // Check if it's a regular file
            file_count++;
        }
    }

    closedir(dp);
    return file_count;
}

void process_directory(const char *input_dir, const char *output_dir) {
    struct dirent *entry;
    DIR *dp = opendir(input_dir);

    if (dp == NULL) {
        perror("Error opening input directory");
        return;
    }

    struct stat st = {0};

    if (stat(output_dir, &st) == -1) {
        if (mkdir(output_dir, 0700) != 0) {
            perror("Error creating output directory");
            closedir(dp);
            return;
        }
    }

    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_REG) {  // Check if it's a regular file
            char input_path[512];
            char output_path[512];

            snprintf(input_path, sizeof(input_path), "%s/%s", input_dir, entry->d_name);
            snprintf(output_path, sizeof(output_path), "%s/%s", output_dir, entry->d_name);

            printf("Processing file: %s\n", entry->d_name);
            flip_bits_in_file(input_path, output_path);
        }
    }

    closedir(dp);
}

int main() {
    char input[10];
    const char *input_dir;
    const char *output_dir;

    while (1) {
        printf("Enter 'flip' to flip bits, 'reverse' to reverse flipped bits, or 'exit' to end the program: ");
        if (scanf("%9s", input) != 1) {
            fprintf(stderr, "Error reading input\n");
            return EXIT_FAILURE;
        }

        if (strcmp(input, "flip") == 0) {
            input_dir = "original";
            output_dir = "flipped";
        } else if (strcmp(input, "reverse") == 0) {
            input_dir = "flipped";
            output_dir = "reversed";
        } else if (strcmp(input, "exit") == 0) {
            printf("Exiting program.\n");
            return EXIT_SUCCESS;
        } else {
            printf("Invalid input. Please enter 'flip', 'reverse', or 'exit'.\n");
            continue;
        }

        int file_count = check_files_in_directory(input_dir);
        if (file_count == 0) {
            printf("No files found in the directory '%s'.\n", input_dir);
            return EXIT_SUCCESS;
        }

        process_directory(input_dir, output_dir);

        printf("All files processed successfully.\n");
    }

    return EXIT_SUCCESS;
}

