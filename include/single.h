#ifndef INCLUDE_SINGLE_H_
#define INCLUDE_SINGLE_H_
#include <stdlib.h>

struct Node;
struct List;

void add_list_element(struct List *lst, const size_t length);
void list_free(struct List *lst);
unsigned char find_most_common_sequence_char(const char*, size_t);

#endif  // INCLUDE_SINGLE_H_
