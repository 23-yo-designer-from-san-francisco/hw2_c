#include "multi.h"
#include "common.c"

#include <pthread.h>
#include <sys/sysinfo.h>
#include <stdbool.h>

typedef struct parcel_node {
    size_t left;
    size_t right;
    const char *str;
    struct parcel_node *next;
} parcel_node;

typedef struct parcel_list {
    parcel_node *first, *last;
} parcel_list;

typedef struct result {
    size_t *frequencies;
    char *representatives;
    size_t length;
} result;

static int thread_result_free(result **res, const size_t chunks) {
    if (res) {
        for (size_t i = 0; i < chunks; ++i) {
            free(res[i]->representatives);
            free(res[i]->frequencies);
            free(res[i]);
        }
        free(res);
        return 0;
    } else {
        return EMPTY_ARG_ERROR;
    }
}

static void parcel_list_free(parcel_list *pl) {
    if (pl) {
        parcel_node *temp = pl->first;
        while (temp != NULL) {
            parcel_node *next = temp;
            temp = temp->next;
            free(next);
        }
    }
}

static void *find_letter_sequences_thread(void *p_node) {
    parcel_node *parcel = (parcel_node *)p_node;
    if (!parcel) {
        pthread_exit(NULL);
    }

    // Список вида
    // freq[0] .... freq[ALPHABET_LENGTH - 1]
    //   |
    //   3 -> 4 -> 3 -> ... // 'a' встретилась 3 раза, затем 4 раза, затем 3 раза

    list *freq = (list *) malloc(sizeof(list) * ALPHABET_LENGTH);
    if (!freq) {
        free(parcel);
        pthread_exit(NULL);
    }
    for (size_t i = 0; i < ALPHABET_LENGTH; ++i) {
        list temp;
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
            if (add_list_element(&freq[cur_char % FIRST_CHAR], length, ALPHABET_LENGTH) != 0) {
                pthread_exit(NULL);
            }
            if (length > max) {
                max = length;
            }
            cur_char = parcel->str[i];
            length = 1;
        }
    }

    add_list_element(&freq[cur_char % FIRST_CHAR], length, ALPHABET_LENGTH);

    if (length > freq[cur_char % FIRST_CHAR].max) {
        freq[cur_char % FIRST_CHAR].max = length;  // максимум ищем сразу
    }
    if (length > max) {
        max = length;
    }

    size_t *frequencies = (size_t *)calloc(max, sizeof(size_t));
    if (!frequencies) {
        free(parcel);
        if (list_free(freq, ALPHABET_LENGTH) != 0) {
            pthread_exit(NULL);
        }
        pthread_exit(NULL);
    }

    char *representatives = (char*)calloc(max, sizeof(char));  // Представители той или иной длины
    if (!representatives) {
        free(parcel);
        if (list_free(freq, ALPHABET_LENGTH) != 0) {
            pthread_exit(NULL);
        }
        free(frequencies);
        pthread_exit(NULL);
    }

    for (size_t i = 0; i < ALPHABET_LENGTH; ++i) {
        node *nd = freq[i].first;
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

    result *res = (result *)malloc(sizeof(result));
    if (!res) {
        free(parcel);
        if (list_free(freq, ALPHABET_LENGTH) != 0) {
            pthread_exit(NULL);
        }
        free(frequencies);
        free(representatives);
        pthread_exit(NULL);
    }
    res->representatives = representatives;
    res->length = max;
    res->frequencies = frequencies;

    if (list_free(freq, ALPHABET_LENGTH) != 0) {
        pthread_exit(NULL);
    }

    pthread_exit((void *)res);
}

static int split_to_cores(parcel_list *pl, const size_t data_length, const char *data) {
    int idle_cpus = get_nprocs();
    size_t left_idx = 0;
    size_t right_idx;
    bool reached_end = 0;

    while (idle_cpus != 0 && left_idx < data_length && !reached_end) {  // делит на потоки (кол-во ядер)
        right_idx = (data_length - left_idx) / idle_cpus + left_idx;
        if (right_idx >= data_length) {
            reached_end = 1;
            right_idx = data_length - 1;
        }
        char last_char = data[right_idx];
        while (right_idx + 1 < data_length && data[right_idx + 1] == last_char && !reached_end) {
            ++right_idx;
        }
        if (pl->first == NULL) {
            pl->first = (parcel_node *) malloc(sizeof(parcel_node));
            if (!pl->first) {
                return MALLOC_ERROR;
            }
            pl->first->next = NULL;
            pl->first->left = left_idx;
            pl->first->right = right_idx;
            pl->first->str = data;
            pl->last = pl->first;
        } else {
            pl->last->next = (parcel_node *) malloc(sizeof(parcel_node));
            if (!pl->last->next) {
                parcel_list_free(pl);
                return MALLOC_ERROR;
            }
            pl->last = pl->last->next;
            pl->last->left = left_idx;
            pl->last->right = right_idx;
            pl->last->str = data;
            pl->last->next = NULL;
        }
        left_idx = right_idx + 1;
        --idle_cpus;
    }
    return get_nprocs() - idle_cpus;
}

unsigned char find_most_common_sequence_char(const char *data, const size_t data_length) {
    if (!data) {
        return EMPTY_ARG_ERROR;
    }

    parcel_list pl;  // "Посылки" для потоков
    pl.first = NULL;
    pl.last = NULL;

    int chunks = split_to_cores(&pl, data_length, data);
    if (chunks <= 0) {
        return SPLIT_ERROR;
    }

    result **results = (result **)malloc(sizeof(result *) * chunks);
    if (!results) {
        parcel_list_free(&pl);
        return CALCULATION_ERROR;
    }
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * chunks);
    if (!threads) {
        parcel_list_free(&pl);
        free(results);
        return CALCULATION_ERROR;
    }

    parcel_node *iter = pl.first;
    for (int i = 0; i < chunks; ++i) {
        pthread_create(&threads[i], NULL, find_letter_sequences_thread, (void *)iter);
        iter = iter->next;
    }

    for (int i = 0; i < chunks; ++i) {
        pthread_join(threads[i], (void **) &results[i]);
    }

    size_t max_length = 0;
    for (int i = 0; i < chunks; ++i) {
        if (!results[i]) {  // Произошла ошибка в одном из потоков
            parcel_list_free(&pl);
            if (thread_result_free(results, chunks)) {
                return CALCULATION_ERROR;
            }
            return CALCULATION_ERROR;
        }
        if (results[i]->length > max_length) {
            max_length = results[i]->length;
        }
    }

    size_t *total_occurrencies = (size_t *)calloc(max_length, sizeof(size_t));
    if (!total_occurrencies) {
        parcel_list_free(&pl);
        if (thread_result_free(results, chunks)) {
            free(threads);
            return CALCULATION_ERROR;
        }
        return CALCULATION_ERROR;
    }
    for (int i = 0; i < chunks; ++i) {
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
    for (int i = 0; i < chunks; ++i) {
        if (max_result <= results[i]->length
            && results[i]->representatives[max_result] != 0
            && results[i]->representatives[max_result] < res) {
            res = results[i]->representatives[max_result];
        }
    }

    if (thread_result_free(results, chunks)) {
        return CALCULATION_ERROR;
    }

    parcel_list_free(&pl);
    free(total_occurrencies);
    free(threads);

    return res;
}
