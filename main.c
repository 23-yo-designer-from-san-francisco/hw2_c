#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <dlfcn.h>
#include <pthread.h>

#include "generator.c"

#define FIRST_CHAR 'a'
#define DATA_SIZE 104857600  // 100 * 1024 * 1024 = 100 МБ
#define FILE_READ_ERR 101
#define MALLOC_ERR 102
#define LIBRARY_ERR 108
#define GENERATE_ERR 109

void *library;
unsigned char (*find_most_common_sequence_char_multi)(const char*, size_t);
unsigned char find_most_common_sequence_char();

typedef struct File_contents {
    char *data;
    size_t length;
} contents;

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
    unsigned char result = '\0';
    unsigned char result_multi = '\0';
    char *data = NULL;
    size_t length = 0;
    struct timeval stop, start;
    contents f;

    library = dlopen("./libmulti.so", RTLD_LAZY);
    if (!library) {
        return LIBRARY_ERR;
    }

    *(void **) (&find_most_common_sequence_char_multi) = dlsym(library, "find_most_common_sequence_char");

    if (argc > 1) {
        f = read_file_contents(argv[1]);
        if (!f.data) {
            printf("File read error\n");
            dlclose(library);
            return FILE_READ_ERR;
        }
        data = f.data;
        length = f.length;
    } else {
        data = generate(DATA_SIZE, FIRST_CHAR);
        if (!data) {
            dlclose(library);
            return GENERATE_ERR;
        }
        length = DATA_SIZE;
    }

    gettimeofday(&start, NULL);
    result = find_most_common_sequence_char(data, length);
    if (result == 0) {
        printf("Malloc error\n");
        free(data);
        dlclose(library);
        return MALLOC_ERR;
    }
    gettimeofday(&stop, NULL);
    printf("Single threaded: %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

    gettimeofday(&start, NULL);
    result_multi = (*find_most_common_sequence_char_multi)(data, length);
    if (result_multi == 0) {
        printf("Malloc error\n");
        free(data);
        dlclose(library);
        return MALLOC_ERR;
    }
    gettimeofday(&stop, NULL);
    printf("Multi threaded: %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

    printf("%c\n%c", result, result_multi);

    dlclose(library);
    free(data);
    pthread_exit(NULL);
}
