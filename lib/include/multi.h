#ifndef LIB_INCLUDE_MULTI_H_
#define LIB_INCLUDE_MULTI_H_
#include <stdlib.h>

struct Parcel_node;
struct Parcel_list;
struct Node;
struct List;
struct Result;

void list_free(struct List*);
void thread_result_free(struct Result **res, size_t chunks);
void add_list_element(struct List *lst, size_t length);
void parcel_list_free(struct Parcel_list *pl);
void *thread(void *parcel_node);
unsigned char find_most_common_sequence_char(const char *data, size_t data_length);


#endif  // LIB_INCLUDE_MULTI_H_
