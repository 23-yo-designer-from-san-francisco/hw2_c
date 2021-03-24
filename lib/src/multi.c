#include "multi.h"
#include <pthread.h>
#include <sys/sysinfo.h>
#include <stdbool.h>

#define ALPHABET_LENGTH 26
#define FIRST_CHAR 'a'
#define MALLOC_ERROR 0

struct Parcel_node {
    size_t left;
    size_t right;
    const char *str;
    struct Parcel_node *next;
};

struct Parcel_list {
    struct Parcel_node *first, *last;
};

struct Node {
    size_t val;
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

void list_free(struct List *lst) {
    if (lst) {
        for (size_t i = 0; i < ALPHABET_LENGTH; ++i) {
            struct Node *nd;
            nd = lst[i].first;
            while (nd != NULL) {
                struct Node *tmp;
                tmp = nd->next;
                free(nd);
                nd = tmp;
            }
        }
        free(lst);
    }
}

void thread_result_free(struct Result **res, const size_t chunks) {
    if (res) {
        for (size_t i = 0; i < chunks; ++i) {
            free(res[i]->representatives);
            free(res[i]->frequencies);
            free(res[i]);
        }
    }
}

void add_list_element(struct List *lst, const size_t length) {
    if (lst) {
        if (lst->first == NULL) {
            lst->first = (struct Node *) malloc(sizeof(struct Node));
            lst->first->val = length;
            lst->first->next = NULL;
            lst->last = lst->first;
        } else {
            lst->last->next = (struct Node *) malloc(sizeof(struct Node));
            lst->last->next->val = length;
            lst->last->next->next = NULL;
            lst->last = lst->last->next;
        }
    }
}

void parcel_list_free(struct Parcel_list *pl) {
    if (pl) {
        struct Parcel_node *temp = pl->first;
        while (temp != NULL) {
            struct Parcel_node *next = temp;
            temp = temp->next;
            free(next);
        }
    }
}

void *thread(void *parcel_node) {
    struct Parcel_node *parcel = (struct Parcel_node *)parcel_node;
    if (!parcel) {
        pthread_exit(NULL);
    }

    // Список вида
    // freq[0] .... freq[ALPHABET_LENGTH - 1]
    //   |
    //   3 -> 4 -> 3 -> ... // 'a' встретилась 3 раза, затем 4 раза, затем 3 раза

    struct List *freq = (struct List *) malloc(sizeof(struct List) * ALPHABET_LENGTH);
    if (!freq) {
        free(parcel);
        pthread_exit(NULL);
    }
    for (size_t i = 0; i < ALPHABET_LENGTH; ++i) {
        struct List temp;
        temp.first = NULL;
        temp.last = NULL;
        temp.max = 0;
        freq[i] = temp;
    }

    size_t max = 0;

    char cur_char = parcel->str[0];
    size_t length = 0;
    for (size_t i = parcel->left; i <= parcel->right; ++i) {
        if (parcel->str[i] == cur_char) {
            ++length;
        } else {
            add_list_element(&freq[cur_char % FIRST_CHAR], length);
            if (length > max) {
                max = length;
            }
            cur_char = parcel->str[i];
            length = 1;
        }
    }

    add_list_element(&freq[cur_char % FIRST_CHAR], length);

    if (length > freq[cur_char % FIRST_CHAR].max) {
        freq[cur_char % FIRST_CHAR].max = length;  // максимум ищем сразу
    }
    if (length > max) {
        max = length;
    }

    size_t *frequencies = (size_t *)calloc(max, sizeof(size_t));
    if (!frequencies) {
        free(parcel);
        list_free(freq);
        pthread_exit(NULL);
    }

    char *representatives = (char*)calloc(max, sizeof(char));  // Представители той или иной длины
    if (!representatives) {
        free(parcel);
        list_free(freq);
        free(frequencies);
        pthread_exit(NULL);
    }

    for (size_t i = 0; i < ALPHABET_LENGTH; ++i) {
        struct Node *nd = freq[i].first;
        while (nd != NULL) {
            if (nd->val != 0) {
                ++frequencies[nd->val - 1];
                if (!representatives[nd->val - 1]) {
                    representatives[nd->val - 1] = FIRST_CHAR + i;
                }
            }
            nd = nd->next;
        }
    }

    struct Result *res = (struct Result *)malloc(sizeof(struct Result));
    if (!res) {
        free(parcel);
        list_free(freq);
        free(frequencies);
        free(representatives);
        pthread_exit(NULL);
    }
    res->representatives = representatives;
    res->length = max;
    res->frequencies = frequencies;

    list_free(freq);

    pthread_exit((void *)res);
}

unsigned char find_most_common_sequence_char(const char *data, const size_t data_length) {
    int idle_cpus = get_nprocs();
    size_t left_idx = 0;
    size_t right_idx;

    struct Parcel_list pl;  // "Посылки" для потоков
    pl.first = NULL;
    pl.last = NULL;

    bool reached_end = 0;
    while (idle_cpus != 0 && left_idx < data_length && !reached_end) {  // делит на потоки (кол-во ядер)
        right_idx = (data_length - left_idx) / idle_cpus + left_idx;
        if (right_idx > data_length) {
            reached_end = 1;
            right_idx = data_length - 1;
        }
            char last_char = data[right_idx];
            while (right_idx + 1 < data_length && data[right_idx + 1] == last_char) {
                ++right_idx;
            }
            if (pl.first == NULL) {
                pl.first = (struct Parcel_node *) malloc(sizeof(struct Parcel_node));
                if (!pl.first) {
                    return MALLOC_ERROR;
                }
                pl.first->next = NULL;
                pl.first->left = left_idx;
                pl.first->right = right_idx;
                pl.first->str = data;
                pl.last = pl.first;
            } else {
                pl.last->next = (struct Parcel_node *) malloc(sizeof(struct Parcel_node));
                if (!pl.last->next) {
                    parcel_list_free(&pl);
                    return MALLOC_ERROR;
                }
                pl.last = pl.last->next;
                pl.last->left = left_idx;
                pl.last->right = right_idx;
                pl.last->str = data;
                pl.last->next = NULL;
            }
            left_idx = right_idx + 1;
            --idle_cpus;
    }

    size_t chunks = get_nprocs() - idle_cpus;

    struct Parcel_node *iter;
    iter = pl.first;

    struct Result **results = (struct Result **)malloc(sizeof(struct Result *) * chunks);
    if (!results) {
        parcel_list_free(&pl);
        return MALLOC_ERROR;
    }
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * chunks);
    if (!threads) {
        parcel_list_free(&pl);
        free(results);
        return MALLOC_ERROR;
    }

    for (size_t i = 0; i < chunks; ++i) {
        pthread_create(&threads[i], NULL, thread, (void *)iter);
        iter = iter->next;
    }

    for (size_t i = 0; i < chunks; ++i) {
        pthread_join(threads[i], (void **) &results[i]);
    }

    size_t max_length = 0;
    for (size_t i = 0; i < chunks; ++i) {
        if (!results[i]) {  // Произошла ошибка в одном из потоков
            parcel_list_free(&pl);
            free(results);
            thread_result_free(results, chunks);
            return MALLOC_ERROR;
        }
        if (results[i]->length > max_length) {
            max_length = results[i]->length;
        }
    }

    size_t *total_occurrencies = (size_t *)calloc(max_length, sizeof(size_t));
    if (!total_occurrencies) {
        parcel_list_free(&pl);
        thread_result_free(results, chunks);
        free(threads);
        return MALLOC_ERROR;
    }
    for (size_t i = 0; i < chunks; ++i) {
        for (size_t j = 0; j < results[i]->length; ++j) {
            total_occurrencies[j] += results[i]->frequencies[j];
        }
    }

    size_t max_result = 0;
    for (size_t i = 0; i < max_length; ++i) {
        if (total_occurrencies[i] > total_occurrencies[max_result]) {
            max_result = i;
        }
    }

    unsigned char res = 255;
    for (size_t i = 0; i < chunks; ++i) {
        if (max_result <= results[i]->length
            && results[i]->representatives[max_result] != 0
            && results[i]->representatives[max_result] < res) {
            res = results[i]->representatives[max_result];
        }
    }

    thread_result_free(results, chunks);
    parcel_list_free(&pl);
    free(total_occurrencies);
    free(threads);
    free(results);

    return res;
}
