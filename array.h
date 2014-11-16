#ifndef ALGO_GISO_ARRAY_H
#define ALGO_GISO_ARRAY_H

#include "stdlib.h"
#include "assert.h"

// int_array

typedef struct int_array {
  int  size;
  int  bufferSize;
  int* array;
} int_array;

int_array int_array_empty(){
  int_array a;
  a.size       = 0;
  a.bufferSize = 0;
  a.array      = NULL;
  return a;
}

int_array int_array_new(int size){
  int_array a;
  a.size       = size;
  a.bufferSize = size;
  a.array      = malloc(size * sizeof(int));
  return a;
}

void int_array_free(int_array *array){
  if(array->array){
    free(array->array);
  }
}

void int_array_append(int_array* array, int value){
  if(array->size < array->bufferSize){
    array->array[array->size] = value;
    array->size += 1;
  }else{
    int nBufferSize = (3 * array->bufferSize) / 2 + 1;
    int* a = malloc(nBufferSize * sizeof(int));
    for(int i = 0; i < array->size; ++i){
      a[i] = array->array[i];
    }
    a[array->size] = value;
    array->size += 1;
    if(array->array){
      free(array->array);
    }
    array->array = a;
    array->bufferSize = nBufferSize;
  }
}

bool int_array_binary_search(int_array* array, int value){
  int lo = 0, hi = array->size-1;
  while(lo != hi){
    int mid = (lo + hi) / 2;
    if(value > array->array[mid]){
      lo = mid + 1;
    }else{
      hi = mid;
    }
  }
  return value == array->array[lo];
}

void int_array_sort(int_array* array, int (*cmp)(int, int)){
  int qsort_cmp(const void* a, const void* b){
    return cmp(* (int*) a, * (int*) b);
  };
  qsort(array->array, array->size, sizeof(int), qsort_cmp);
}

void int_array_sort_less(int_array* array){
  int cmp(int a, int b){
    return a - b;
  }
  int_array_sort(array, cmp);
}

int int_array_compare(int_array* a, int_array* b){
  assert(a != NULL && b != NULL);
  for(int i = 0; i < a->size && i < b->size; ++i){
    if(a->array[i] < b->array[i]){
      return -1;
    }else if(a->array[i] > b->array[i]){
      return 1;
    }
  }
  return b->size - a->size;
}

// int_array_array

typedef struct int_array_array{
  int        size;
  int        bufferSize;
  int_array* array;
} int_array_array;

int_array_array int_array_array_empty(){
  int_array_array a;
  a.size       = 0;
  a.bufferSize = 0;
  a.array      = NULL;
  return a;
}

int_array_array int_array_array_new(int size){
  int_array_array a;
  a.size       = size;
  a.bufferSize = size;
  a.array      = malloc(size * sizeof(int_array));
  for(int i = 0; i < size; ++i){
    a.array[i] = int_array_empty();
  }
  return a;
}

void int_array_array_free(int_array_array *array){
  for(int i = 0; i < array->size; ++i){
    int_array_free(&array->array[i]);
  }
  free(array->array);
}

void int_array_array_append(int_array_array* array, int_array value){
  if(array->size < array->bufferSize){
    array->array[array->size] = value;
    array->size += 1;
  }else{
    int nBufferSize = (3 * array->bufferSize) / 2 + 1;
    int_array* a = malloc(nBufferSize * sizeof(int_array));
    for(int i = 0; i < array->size; ++i){
      a[i] = array->array[i];
    }
    a[array->size] = value;
    array->size += 1;
    if(array->array){
      free(array->array);
    }
    array->array = a;
    array->bufferSize = nBufferSize;
  }
}

void int_array_array_sort(int_array_array* array, int (*cmp)(int_array*, int_array*)){
  int qsort_cmp(const void* a, const void* b){
    return cmp((int_array*) a, (int_array*) b);
  };
  qsort(array->array, array->size, sizeof(int), qsort_cmp);
}

void int_array_array_sort_less(int_array_array* array){
  int_array_array_sort(array, int_array_compare);
}

#endif
