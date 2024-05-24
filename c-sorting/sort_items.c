#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 100
#define MAX_LENGTH 100

void readItemsFromFile(char *filePath, char items[][MAX_LENGTH], int *itemCount) {
    printf("Opening file for reading: %s\n", filePath);
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }

    *itemCount = 0;
    printf("Reading items from file...\n");
    while (fgets(items[*itemCount], MAX_LENGTH, file) != NULL && *itemCount < MAX_ITEMS) {
        // Remove newline character
        size_t length = strlen(items[*itemCount]);
        if (length > 0 && items[*itemCount][length - 1] == '\n') {
            items[*itemCount][length - 1] = '\0';
        }
        //printf("Read item: %s\n", items[*itemCount]);
        (*itemCount)++;
    }

    printf("Total items read: %d\n", *itemCount);
    fclose(file);
}

int compare(const void *a, const void *b) {
    const char *strA = (const char *)a;
    const char *strB = (const char *)b;
    return strcmp(strA, strB);
}

void sortItems(char items[][MAX_LENGTH], int itemCount) {
    printf("Sorting items...\n");
    for (int i = 0; i < itemCount; i++) {
        //printf("Before sort: %s\n", items[i]);
    }
    qsort(items, itemCount, sizeof(items[0]), compare);
    printf("Items sorted.\n");
    for (int i = 0; i < itemCount; i++) {
        //printf("After sort: %s\n", items[i]);
    }
}

void writeItemsToFile(char *filePath, char items[][MAX_LENGTH], int itemCount) {
    printf("Opening file for writing: %s\n", filePath);
    FILE *file = fopen(filePath, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    printf("Writing items to file...\n");
    for (int i = 0; i < itemCount; i++) {
        fprintf(file, "%s\n", items[i]);
        //printf("Wrote item: %s\n", items[i]);
    }

    fclose(file);
    printf("Finished writing items to file.\n");
}

int main() {
    char inputFilePath[] = "list/items.txt"; // Change this to your file path
    char outputFilePath[] = "list/sortedlist.txt"; // Change this to your desired output path
    char items[MAX_ITEMS][MAX_LENGTH];
    int itemCount;

    printf("Starting program...\n");

    printf("Calling readItemsFromFile...\n");
    readItemsFromFile(inputFilePath, items, &itemCount);
    printf("readItemsFromFile completed.\n");

    printf("Calling sortItems...\n");
    sortItems(items, itemCount);
    printf("sortItems completed.\n");

    printf("Calling writeItemsToFile...\n");
    writeItemsToFile(outputFilePath, items, itemCount);
    printf("writeItemsToFile completed.\n");

    printf("Sorted list saved to %s\n", outputFilePath);
    printf("Program finished.\n");

    return 0;
}
