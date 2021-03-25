#include "single.h"

#define ALPHABET_LENGTH 26
#define FIRST_CHAR 'a'
#define MALLOC_ERROR 102
#define EMPTY_ARG_ERROR 255
#define CALCULATION_ERROR 254

struct node {
    size_t val;
    node *next;
};

struct list {
    node *first, *last;
    size_t max;
};

void list_free(list *lst) {
    if (lst) {
        for (size_t i = 0; i < ALPHABET_LENGTH; ++i) {
            node *nd = lst[i].first;
            while (nd != NULL) {
                node *tmp = nd->next;
                free(nd);
                nd = tmp;
            }
        }
        free(lst);
    }
}

int add_list_element(list *lst, const size_t length) {
    if (lst) {
        if (lst->first == NULL) {
            lst->first = (node *) malloc(sizeof(node));
            if (!lst->first) {
                return MALLOC_ERROR;
            }
            lst->first->val = length;
            lst->first->next = NULL;
            lst->last = lst->first;
        } else {
            lst->last->next = (node *) malloc(sizeof(node));
            if (!lst->last->next) {
                list_free(lst);
                return MALLOC_ERROR;
            }
            lst->last->next->val = length;
            lst->last->next->next = NULL;
            lst->last = lst->last->next;
        }
        return 0;
    } else {
        return EMPTY_ARG_ERROR;
    }
}

unsigned char find_most_common_sequence_char(const char *data, const size_t data_length) {
    if (!data) {
        return EMPTY_ARG_ERROR;
    }
    // Список вида
    // freq[0] .... freq[ALPHABET_LENGTH - 1]
    //   |
    //   3 -> 4 -> 3 -> ... // FIRST_CHAR встретилась 3 раза, затем 4 раза, затем 3 раза

    list *freq_list = (list *) malloc(sizeof(list) * ALPHABET_LENGTH);
    if (!freq_list) {
        return MALLOC_ERROR;
    }
    for (size_t i = 0; i < ALPHABET_LENGTH; ++i) {
        list temp;
        temp.first = NULL;
        temp.last = NULL;
        temp.max = 0;
        freq_list[i] = temp;
    }

    size_t max = 0;

    char cur_char = data[0];
    size_t length = 1;
    for (size_t i = 1; i < data_length; ++i) {
        if (data[i] == cur_char) {
            ++length;
        } else {
            if (add_list_element(&freq_list[cur_char % FIRST_CHAR], length)) {
                list_free(freq_list);
                return CALCULATION_ERROR;
            }
            if (length > max) {
                max = length;
            }
            cur_char = data[i];
            length = 1;
        }
    }

    if (add_list_element(&freq_list[cur_char % FIRST_CHAR], length)) {
        list_free(freq_list);
        return CALCULATION_ERROR;
    }

    if (length > freq_list[cur_char % FIRST_CHAR].max) {
        freq_list[cur_char % FIRST_CHAR].max = length;
    }
    if (length > max) {
        max = length;
    }

    size_t *letter_frequencies = (size_t *)calloc(max, sizeof(size_t));  // Массив частот
    if (!letter_frequencies) {
        list_free(freq_list);
        return MALLOC_ERROR;
    }

    size_t max_freq = 0;
    for (size_t i = 0; i < ALPHABET_LENGTH; ++i) {
        node *nd = freq_list[i].first;
        while (nd != NULL) {
            ++letter_frequencies[nd->val - 1];
            if (letter_frequencies[nd->val - 1] > letter_frequencies[max_freq]) {
                max_freq = nd->val - 1;
            }
            nd = nd->next;
        }
    }

    ++max_freq;  // Из-за сдвига индексов
    for (size_t i = 0; i < ALPHABET_LENGTH; ++i) {
        node *nd = freq_list[i].first;
        while (nd != NULL) {
            if (nd->val == max_freq) {
                list_free(freq_list);
                free(letter_frequencies);
                return FIRST_CHAR + i;
            }
            nd = nd->next;
        }
    }

    list_free(freq_list);
    free(letter_frequencies);
    return 0;
}
