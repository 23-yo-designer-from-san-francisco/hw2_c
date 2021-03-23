#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DATA_SIZE 104857600  // 100 * 1024 * 1024 = 100 МБ

char find_most_common_sequence_char();

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

int main(int argc, char** argv) {
    char result = '\0';
    if (argc > 1) {
        result = find_most_common_sequence_char(argv[1], strlen(argv[1]));
    } else {
        char *data = generate(DATA_SIZE);
        result = find_most_common_sequence_char(data, DATA_SIZE);
        free(data);
    }
    printf("%c", result);

    return 0;
}
