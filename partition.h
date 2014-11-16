#ifndef ALGO_GISO_PARTITION_H
#define ALGO_GISO_PARTITION_H

#include "stdlib.h"
#include "stdbool.h"

typedef struct partition {
  int_array_array partition;
  int_array       elements;
} partition;

partition partition_empty(){
  partition p;
  p.partition = int_array_array_empty();
  p.elements = int_array_empty();
  return p;
}

bool partition_is_empty(partition* p){
  assert(p != NULL);
  return p->partition.size == 0 && p->elements.size == 0;
}

partition partition_new(int size){
  partition p;
  p.partition = int_array_array_empty();
  p.elements = int_array_new(size);
  for(int i = 0; i < size; ++i){
    p.elements.array[i] = -1;
  }
  return p;
}

partition partition_new_with_classes(int size, int cls_size){
  partition p;
  p.partition = int_array_array_new(cls_size);
  p.elements = int_array_new(size);
  for(int i = 0; i < size; ++i){
    p.elements.array[i] = -1;
  }
  return p;
}

void partition_free(partition* p){
  assert(p != NULL);
  int_array_array_free(&p->partition);
  int_array_free(&p->elements);
}

void partition_cleanup(partition* p){
  assert(p != NULL);
  int_array_array npartition = int_array_array_empty();
  for(int i = 0; i < p->partition.size; ++i){
    if(p->partition.array[i].size != 0){
      for(int j = 0; j < p->partition.array[i].size; ++j){
        p->elements.array[p->partition.array[i].array[j]] = npartition.size;
      }
      int_array_array_append(&npartition, p->partition.array[i]);
      p->partition.array[i] = int_array_empty();
    }
  }
  int_array_array_free(&p->partition);
  p->partition = npartition;
}

int partition_new_class(partition* p){
  assert(p != NULL);
  int cls = p->partition.size;
  int_array_array_append(&p->partition, int_array_empty());
  return cls;
}

void partition_clear_class(partition* p, int cls){
  assert(p != NULL);
  assert(cls >= 0 && cls < p->partition.size);
  for(int i = 0; i < p->partition.array[cls].size; ++i){
    p->elements.array[p->partition.array[cls].array[i]] = -1;
  }
  int_array_free(&p->partition.array[cls]);
  p->partition.array[cls] = int_array_empty();
}

void partition_set_class(partition* p, int el, int cls){
  assert(p != NULL);
  assert(el >= 0 && el < p->elements.size);
  assert(cls >= 0 && cls < p->partition.size);

  p->elements.array[el] = cls;
  int_array_append(&p->partition.array[cls], el);
}

#endif
