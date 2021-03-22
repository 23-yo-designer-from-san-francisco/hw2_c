//
// Created by test on 3/21/21.
//

#include "single.h"
#include <stdio.h>

#define ALPHABET_LENGTH 26

struct Node {
    int val;
    struct Node *next;
};

struct List {
    struct Node *first, *last;
    size_t max;
};

char find_most_common_sequence_char(const char *data, const size_t data_length) {
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

    char cur_char = data[0];
    size_t length = 1;
    for (size_t i = 1; i < data_length; ++i) {
        if (data[i] == cur_char) {
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
            cur_char = data[i];
            length = 1;
        }
    }

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
        freq[cur_char % 'a'].max = length;
    }
    if (length > max) {
        max = length;
    }

//    char *representatives = (char*)calloc(max, sizeof(char));
//
//    for (size_t i = 0; i < ALPHABET_LENGTH; ++i) {
//        struct Node *nd = freq[i].first;
//        while (nd != NULL) {
//            representatives[nd->val - 1] = 'a' + i;
//            nd = nd->next;
//        }
//    }
//
//    for (size_t i = 0; i < max; ++i) {
//        printf("'%c' represents %zu repetitions\n", representatives[i], i + 1);
//    }

    for (size_t i = 0; i < ALPHABET_LENGTH; ++i) {

        struct Node *nd = freq[i].first;
        while(nd != NULL) {
            if (nd->val == max) {
                return 'a' + i;
            }
            nd = nd->next;
        }
    }

//    printf("Длинное: %zu", max);
//    char answer = representatives[max-1];
//    return answer;
    return 0;
}