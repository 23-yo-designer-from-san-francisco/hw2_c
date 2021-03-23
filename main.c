#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <pthread.h>

#include "single.h"
#include "multi.h"

//#define DATA_SIZE 104857600 // 100 * 1024 * 1024 = 100 МБ

char *generate(size_t size) {
        srand(time(NULL));
    char *data = (char *)malloc(sizeof(char) * size);
    char cur_char = 'a';
    _Bool prev_changed = 0;
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

int main() {
    size_t DATA_SIZE = 1000000;
    char *data = generate(DATA_SIZE);
//    size_t DATA_SIZE = 52;
//    char *data = "aaaaaabbhhhddkkkkkkuuuujjjjhhhggppppppppppppllpppppp";
    time_t single_start, single_end, multi_start, multi_end;

    time(&multi_start);
    char result = find_most_common_sequence_char_multi(data, DATA_SIZE);
    time(&multi_end);
    time(&single_start);
    char result2 = find_most_common_sequence_char(data, DATA_SIZE); //выдает первый
    time(&single_end);
//    printf("Generated string: %s\n", data);

    printf("%c %c", result, result2);

//    printf("Multi was %lld times faster than single\n", (long long) (single_end-single_start)/
//           (long long) (multi_end-multi_start));

//    free(data);
    return 0;
}
