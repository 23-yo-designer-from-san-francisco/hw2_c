#ifndef LIB_INCLUDE_MULTI_H_
#define LIB_INCLUDE_MULTI_H_
#include <stdlib.h>

struct parcel_node;
struct parcel_list;
struct node;
struct list;
struct result;

typedef struct parcel_node parcel_node;
typedef struct parcel_list parcel_list;
typedef struct node node;
typedef struct list list;
typedef struct result result;

int list_free(struct list*);
int thread_result_free(struct result **res, size_t chunks);
int add_list_element(struct list *lst, size_t length);
void parcel_list_free(struct parcel_list *pl);
void *thread(void *parcel_node);
int split_to_cores(parcel_list *pl, const size_t data_length, const char *data);
unsigned char find_most_common_sequence_char(const char *data, size_t data_length);


#endif  // LIB_INCLUDE_MULTI_H_
