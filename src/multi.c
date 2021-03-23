#include "../include/multi.h"
#include <stdio.h>
#include <pthread.h>
#include <sys/sysinfo.h>

#define ALPHABET_LENGTH 26

struct Parcel_node {
    size_t left;
    size_t right;
    char *str;
    struct Parcel *next;
};

struct Parcel_list {
    struct Parcel_node *first, *last;
};

/////////////////////////

struct Node {
    int val;
    struct Node *next;
};

struct List {
    struct Node *first, *last;
    size_t max;
};

struct Result {
    size_t *frequencies;
    char *representatives;
    size_t length;
};

void *thread(void *parcel_node) {
    struct Parcel_node *p = (struct Parcel_node *)parcel_node;

    //Список вида
    //freq[0] .... freq[ALPHABET_LENGTH - 1]
    //   |
    //   3 -> 4 -> 3 -> ... // 'a' встретилась 3 раза, затем 4 раза, затем 3 раза

    //Инициализация массива
    struct List *freq = (struct List *) malloc(sizeof(struct List) * ALPHABET_LENGTH);
    for (size_t i = 0; i < ALPHABET_LENGTH; ++i) {
        struct List temp;
        temp.first = NULL;
        temp.last = NULL;
        temp.max = 0;
        freq[i] = temp;
    }

    size_t max = 0;

    char cur_char = p->str[0];
    size_t length = 0;
    for (size_t i = p->left; i <= p->right; ++i) {
        if (p->str[i] == cur_char) {
            ++length;
        } else {
            if (freq[cur_char % 'a'].first == NULL) {
                freq[cur_char % 'a'].first = (struct Node *) malloc(sizeof(struct Node));
                freq[cur_char % 'a'].first->val = length;
                freq[cur_char % 'a'].first->next = NULL;
                freq[cur_char % 'a'].last = freq[cur_char % 'a'].first;
            } else {
                freq[cur_char % 'a'].last->next = (struct Node *) malloc(sizeof(struct Node));
                freq[cur_char % 'a'].last->next->val = length;
                freq[cur_char % 'a'].last->next->next = NULL;
                freq[cur_char % 'a'].last = freq[cur_char % 'a'].last->next;

            }
            if (length > max) {
                max = length;
            }
            cur_char = p->str[i];
            length = 1;
        }
    }

    printf("Length: %zu\n", length);

    if (freq[cur_char % 'a'].first == NULL) {
        freq[cur_char % 'a'].first = (struct Node *) malloc(sizeof(struct Node));
        freq[cur_char % 'a'].first->val = length;
        freq[cur_char % 'a'].first->next = NULL;
        freq[cur_char % 'a'].last = freq[cur_char % 'a'].first;
    } else {
        freq[cur_char % 'a'].last->next = (struct Node *) malloc(sizeof(struct Node));
        freq[cur_char % 'a'].last->next->val = length;
        freq[cur_char % 'a'].last->next->next = NULL;
        freq[cur_char % 'a'].last = freq[cur_char % 'a'].last->next;
    }
    if (length > freq[cur_char % 'a'].max) {
        freq[cur_char % 'a'].max = length; //максимум ищем сразу
    }
    if (length > max) {
        max = length;
    }

    size_t *frequencies = (size_t *)calloc(max, sizeof(size_t)); //Частоты


    char *representatives = (char*)calloc(max, sizeof(char)); //Представители той или иной длины

    for (size_t i = 0; i < ALPHABET_LENGTH; ++i) {  //Ищем представителя для каждой длины, получаем разные ответы
        struct Node *nd = freq[i].first;
        while (nd != NULL) {
            ++frequencies[nd->val - 1];
            if (!representatives[nd->val - 1]) {
                representatives[nd->val - 1] = 'a' + i;
            }
            nd = nd->next;
        }
    }

    for (size_t i = 0; i < max; ++i) {
        printf("'%c' represents %zu repetitions\n", representatives[i], i + 1);
    }

    printf("Max: %zu\t Representative: %c\n", max, representatives[max-1]);
    struct Result *res = (struct Result *)malloc(sizeof(struct Result));
    res->representatives = representatives;
    res->length = max;
    res->frequencies = frequencies;

    pthread_exit((void *)res);
}

char find_most_common_sequence_char_multi(char *data, size_t data_length) {
    int split = get_nprocs();
    size_t left_idx = 0;
    size_t right_idx;

    struct Parcel_list pl;
    pl.first = NULL;
    pl.last = NULL;

    while (split != 0 && left_idx < data_length) { //делит на ядра
        right_idx = (data_length - left_idx) / split + left_idx;
        char last_char = data[right_idx];
        while (right_idx + 1 < data_length && data[right_idx + 1] == last_char) {
            ++right_idx;
        }
        if (right_idx == data_length) {
            --right_idx;
        }
        if (pl.first == NULL) {
            pl.first = (struct Parcel_node *)malloc(sizeof(struct Parcel_node));
            pl.first->next = NULL;
            pl.first->left = left_idx;
            pl.first->right = right_idx;
            pl.first->str = data;
            pl.last = pl.first;
        } else {
            pl.last->next = (struct Parcel_node*)malloc(sizeof(struct Parcel_node));
            pl.last = pl.last->next;
            pl.last->left = left_idx;
            pl.last->right = right_idx;
            pl.last->str = data;
        }
        printf("Chunk: [%zu:%zu]\n", pl.last->left, pl.last->right);
        left_idx = right_idx + 1;
        --split;
    }

    size_t chunks = get_nprocs() - split;

    struct Parcel_node *iter;
    iter = pl.first;

    struct Result **results = (struct Result **)malloc(sizeof(struct Result *) * chunks);
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * chunks);

    for (size_t i = 0; i < chunks; ++i) {
        pthread_create(&threads[i], NULL, thread, (void *)iter);
        iter = iter->next;
    }

    for (size_t i = 0; i < chunks; ++i) {
        pthread_join(threads[i], (void **) &results[i]);
    }

    for (size_t i = 0; i < chunks; ++i) {
        printf("Result of thread %zu: \n", i);
        for (size_t j = 0; j < results[i]->length; ++j) {
            printf("freq[%zu]: %zu\n ", j+1, results[i]->frequencies[j]);
        }
        printf("\n");
    }

    size_t max_length = 0;
    for (size_t i = 0; i < chunks; ++i) {
        if (results[i]->length > max_length) {
            max_length = results[i]->length;
        }
    }

    printf("Max_length: %zu\n", max_length);

    size_t *total = (size_t *)calloc(max_length, sizeof(size_t));
    for (size_t i = 0; i < chunks; ++i) {
        for (size_t j = 0; j < results[i]->length; ++j) {
            total[j] += results[i]->frequencies[j];
        }
    }

    printf("[");
    size_t max_result = 0;
    for (size_t i = 0; i < max_length; ++i) {
        if (total[i] > total[max_result]) {
            max_result = i;
        }
        printf("%zu ", total[i]);
    }
    printf("]\nMax result: %zu\n", max_result);

    for (size_t i = 0; i < chunks; ++i) {
        if (max_result <= results[i]->length && results[i]->representatives[max_result] != 0) {
            return results[i]->representatives[max_result];
        }
    }
    return 0;
}