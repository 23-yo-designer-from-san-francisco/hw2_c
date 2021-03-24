#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DATA_SIZE 104857600  // 100 * 1024 * 1024 = 100 МБ
#define FILE_READ_ERR 101
#define MALLOC_ERR 102

char find_most_common_sequence_char();

struct File_contents {
    char *data;
    size_t length;
};

char *generate(size_t size) {
    srand(time(NULL));
    char *data = (char *)malloc(sizeof(char) * size);
    char cur_char = 'a';
    _Bool prev_changed = 0;  // Комбинации длины >= 2
    for (size_t i = 0; i < size; ++i) {
        if (rand() & 1 && !prev_changed) {
            prev_changed = 1;
            ++cur_char;
            if (cur_char > 'z') {
                cur_char = 'a';
            }
        } else {
            prev_changed = 0;
        }
        data[i] = cur_char;
    }
    return data;
}

struct File_contents read_file_contents(const char *filename) {
    FILE *fptr;
    char *data;
    long length;
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
        return contents;
    }
    if (fread(data, 1, length, fptr) != length) {
        printf("File read error\n");
        free(data);
        contents.data = NULL;
        return contents;
    }
    if (fptr) {
        fclose(fptr);
    }

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
        char *data = generate(DATA_SIZE);
        result = find_most_common_sequence_char(data, DATA_SIZE);
        free(data);
    }
    printf("%c", result);

    return 0;
}
