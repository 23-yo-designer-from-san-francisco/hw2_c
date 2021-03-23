#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DATA_SIZE 104857600  // 100 * 1024 * 1024 = 100 МБ

int main(int argc, char **argv) {
    if (argc > 1) {
        srand(time(NULL));
        char *data = (char *) malloc(sizeof(char) * DATA_SIZE);
        char cur_char = *argv[1];
        _Bool prev_changed = 0;  // Комбинации длины >= 2
        for (size_t i = 0; i < DATA_SIZE; ++i) {
            if (rand() & 1 && !prev_changed) {
                prev_changed = 1;
                ++cur_char;
                if (cur_char > 'z') {
                    cur_char = *argv[1];
                }
            } else {
                prev_changed = 0;
            }
            data[i] = cur_char;
        }
        printf("%s", data);
    }
    return 0;
}