#include "../include/multi.h"
#include <stdio.h>
#include <pthread.h>
#include <sys/sysinfo.h>

#define ALPHABET_LENGTH 26
#define FIRST_CHAR 'a'

struct Parcel_node {
    size_t left;
    size_t right;
    char *str;
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

    // Список вида
    // freq[0] .... freq[ALPHABET_LENGTH - 1]
    //   |
    //   3 -> 4 -> 3 -> ... // 'a' встретилась 3 раза, затем 4 раза, затем 3 раза

    struct List *freq = (struct List *) malloc(sizeof(struct List) * ALPHABET_LENGTH);
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

    char *representatives = (char*)calloc(max, sizeof(char));  // Представители той или иной длины

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
    res->representatives = representatives;
    res->length = max;
    res->frequencies = frequencies;

    list_free(freq);

    pthread_exit((void *)res);
}

char find_most_common_sequence_char(char *data, size_t data_length) {
    int idle_cpus = get_nprocs();
    size_t left_idx = 0;
    size_t right_idx;

    struct Parcel_list pl;
    pl.first = NULL;
    pl.last = NULL;

    while (idle_cpus != 0 && left_idx < data_length) {  // делит на ядра
        right_idx = (data_length - left_idx) / idle_cpus + left_idx;
        if (right_idx < data_length) {
            char last_char = data[right_idx];
            while (right_idx + 1 < data_length && data[right_idx + 1] == last_char) {
                ++right_idx;
            }
            if (right_idx == data_length) {
                --right_idx;
            }
            if (pl.first == NULL) {
                pl.first = (struct Parcel_node *) malloc(sizeof(struct Parcel_node));
                pl.first->next = NULL;
                pl.first->left = left_idx;
                pl.first->right = right_idx;
                pl.first->str = data;
                pl.last = pl.first;
            } else {
                pl.last->next = (struct Parcel_node *) malloc(sizeof(struct Parcel_node));
                pl.last = pl.last->next;
                pl.last->left = left_idx;
                pl.last->right = right_idx;
                pl.last->str = data;
            }
            left_idx = right_idx + 1;
            --idle_cpus;
        } else {
            break;
        }
    }

    size_t chunks = get_nprocs() - idle_cpus;

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

    size_t max_length = 0;
    for (size_t i = 0; i < chunks; ++i) {
        if (results[i]->length > max_length) {
            max_length = results[i]->length;
        }
    }

    size_t *total = (size_t *)calloc(max_length, sizeof(size_t));
    for (size_t i = 0; i < chunks; ++i) {
        for (size_t j = 0; j < results[i]->length; ++j) {
            total[j] += results[i]->frequencies[j];
        }
    }

    size_t max_result = 0;
    for (size_t i = 0; i < max_length; ++i) {
        if (total[i] > total[max_result]) {
            max_result = i;
        }
    }

    for (size_t i = 0; i < chunks; ++i) {
        if (max_result <= results[i]->length && results[i]->representatives[max_result] != 0) {
            char res = results[i]->representatives[max_result];

            thread_result_free(results, chunks);
            parcel_list_free(&pl);
            free(total);
            free(threads);
            free(results);

            return res;
        }
    }

    thread_result_free(results, chunks);
    parcel_list_free(&pl);
    free(total);
    free(threads);
    free(results);

    return 0;
}
