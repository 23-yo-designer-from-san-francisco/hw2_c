#define ALPHABET_LENGTH 26

#define FIRST_CHAR 'a'
#define CALCULATION_ERROR 254

#define MALLOC_ERROR 102
#define EMPTY_ARG_ERROR 255
#define SPLIT_ERROR 108

typedef struct node {
    size_t val;
    struct node *next;
} node;

typedef struct list {
    node *first, *last;
    size_t max;
} list;

int list_free(list *lst, const size_t SIZE) {
    if (lst) {
        for (size_t i = 0; i < SIZE; ++i) {
            node *nd;
            nd = lst[i].first;
            while (nd != NULL) {
                node *tmp;
                tmp = nd->next;
                free(nd);
                nd = tmp;
            }
        }
        free(lst);
        return 0;
    } else {
        return EMPTY_ARG_ERROR;
    }
}

int add_list_element(list *lst, const size_t length, const size_t SIZE) {
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
                int res;
                if (res = list_free(lst, SIZE), res != 0) {
                    return res;
                }
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

