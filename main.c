#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "generator.c"

#define FIRST_CHAR 'a'
#define DATA_SIZE 104857600  // 100 * 1024 * 1024 = 100 МБ
#define FILE_READ_ERR 101
#define MALLOC_ERR 102


char find_most_common_sequence_char();

struct File_contents {
    char *data;
    size_t length;
};

struct File_contents read_file_contents(const char *filename) {
    FILE *fptr;
    char *data;
    size_t length;
    struct File_contents contents;

    fptr = fopen(filename, "rb");
    if (!fptr) {
        printf("File does not exist\n");
        contents.data = NULL;
        return contents;
    }
    fseek(fptr, 0L, SEEK_END);
    length = ftell(fptr);
    rewind(fptr);
    data = (char*)malloc((length+1));
    if (!data) {
        printf("Memory allocation error!\n");
        contents.data = NULL;
        fclose(fptr);
        return contents;
    }
    if (fread(data, 1, length, fptr) != length) {
        printf("File read error\n");
        free(data);
        contents.data = NULL;
        fclose(fptr);
        return contents;
    }
    fclose(fptr);

    contents.length = length;
    contents.data = data;
    return contents;
}

int main(int argc, char** argv) {
    char result = '\0';
    if (argc > 1) {
        struct File_contents f = read_file_contents(argv[1]);
        if (!f.data) {
            printf("File read error\n");
            return FILE_READ_ERR;
        }
        result = find_most_common_sequence_char(f.data, f.length);
        if (result == 0) {
            printf("Malloc error\n");
            free(f.data);
            return MALLOC_ERR;
        }
        free(f.data);
    } else {
        char *data = generate(DATA_SIZE, FIRST_CHAR);
        result = find_most_common_sequence_char(data, DATA_SIZE, FIRST_CHAR);
        free(data);
    }
    printf("%c", result);

    return 0;
}
