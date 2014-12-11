#ifndef ALGO_GISO_WL_PARTITION_H
#define ALGO_GISO_WL_PARTITION_H

#include "stdlib.h"
#include "stdbool.h"
#include "array.h"
#include "set.h"

typedef struct wl_partition {
  int_array_array partition;
  int_array       elements_a;
  int_array       elements_b;
  int_set         update_queue;
} partition;

partition wl_partition_empty();
bool wl_partition_is_empty(partition* p);

/* partition partition_new(int size); */
/* partition partition_new_with_classes(int size, int cls_size); */
/* void partition_free(partition* p); */
/* partition partition_copy(partition* p); */
/* void partition_cleanup(partition* p); */
/* int partition_new_class(partition* p); */
/* void partition_clear_class(partition* p, int cls); */
/* void partition_set_class(partition* p, int el, int cls); */

#endif
