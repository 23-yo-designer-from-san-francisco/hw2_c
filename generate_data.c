#include "generator.c"

#define DATA_SIZE 104857600  // 100 МБ
#define MALLOC_ERR 102

int main(int argc, char **argv) {
    if (argc > 1) {
        char *data = generate(DATA_SIZE, *argv[1]);
        if (!data) {
            return MALLOC_ERR;
        }
        printf("%s", data);
        free(data);
    }
    return 0;
}
