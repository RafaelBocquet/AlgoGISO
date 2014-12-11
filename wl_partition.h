#ifndef ALGO_GISO_WL_PARTITION_H
#define ALGO_GISO_WL_PARTITION_H

#include "stdlib.h"
#include "stdbool.h"
#include "array.h"
#include "set.h"

typedef struct wl_partition {
  
  int_array_pair_array partition;
  int_array            elements[2];
  int_set              update_queue;
} wl_partition;

wl_partition wl_partition_empty();
bool wl_partition_is_empty(wl_partition* p);
wl_partition wl_partition_new(int size);
wl_partition wl_partition_new_with_classes(int size, int cls_size);
void wl_partition_free(wl_partition* p);
wl_partition wl_partition_copy(wl_partition* p);
bool wl_partition_cleanup(wl_partition* p);
int wl_partition_new_class(wl_partition* p);
void wl_partition_set_class(wl_partition* p, int cls, int a[2]);
void wl_partition_set_class_single(wl_partition* p, int cls, int i, int a);
/* int partition_new_class(partition* p); */
/* void partition_clear_class(partition* p, int cls); */
/* void partition_set_class(partition* p, int el, int cls); */

#endif
