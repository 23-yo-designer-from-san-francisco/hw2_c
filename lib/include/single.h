#ifndef LIB_INCLUDE_SINGLE_H_
#define LIB_INCLUDE_SINGLE_H_
#include <stdlib.h>

struct node;
struct list;

typedef struct node node;
typedef struct list list;

int add_list_element(list *lst, const size_t length);
void list_free(list *lst);
unsigned char find_most_common_sequence_char(const char*, size_t);

#endif  // LIB_INCLUDE_SINGLE_H_
