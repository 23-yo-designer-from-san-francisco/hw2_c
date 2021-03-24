#ifndef LIB_INCLUDE_SINGLE_H_
#define LIB_INCLUDE_SINGLE_H_
#include <stdlib.h>

struct Node;
struct List;

void add_list_element(struct List *lst, const size_t length);
void list_free(struct List *lst);
unsigned char find_most_common_sequence_char(const char*, size_t);

#endif  // LIB_INCLUDE_SINGLE_H_
