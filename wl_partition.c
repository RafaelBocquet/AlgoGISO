#include "wl_partition.h"

wl_partition wl_partition_empty(){
  wl_partition p;
  p.partition    = int_array_array_empty();
  p.elements_a   = int_array_empty();
  p.elements_b   = int_array_empty();
  p.update_queue = int_set_empty();
  return p;
}

bool wl_partition_is_empty(wl_partition* p){
  assert(p != NULL);
  return p->partition.size == 0 && p->elements_a.size == 0;
}

void wl_partition_free(wl_partition* p){
  assert(p != NULL);
}
