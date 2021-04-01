#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define FILE_ERROR 123

int save_seed(const time_t seed) {
    FILE *fptr = fopen("seed", "wb");
    if (!fptr) {
        return FILE_ERROR;
    }
    if (fwrite(&seed, sizeof(time_t), 1, fptr) != 1) {
        fclose(fptr);
        return FILE_ERROR;
    }
    fclose(fptr);
    return 0;
}

char *generate(const size_t DATA_SIZE, const char start) {
    const time_t seed = time(NULL);
    srand(seed);
    if (save_seed(seed) != 0) {
        return NULL;
    }
    char *data = (char *) malloc(sizeof(char) * DATA_SIZE);
    if (!data) {
        return NULL;
    }
    char cur_char = start;
    _Bool prev_changed = 0;  // Комбинации длины >= 2
    for (size_t i = 0; i < DATA_SIZE; ++i) {
        if (rand() & 1 && !prev_changed) {
            prev_changed = 1;
            ++cur_char;
            if (cur_char > 'z') {
                cur_char = start;
            }
        } else {
            prev_changed = 0;
        }
        data[i] = cur_char;
    }
    return data;
}