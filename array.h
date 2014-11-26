#ifndef ALGO_GISO_ARRAY_H
#define ALGO_GISO_ARRAY_H

#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "util.h"

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

int_array int_array_copy(int_array* array){
  int_array a;
  a.size = array->size;
  a.bufferSize = a.size;
  if(a.size != 0){
    a.array = malloc(a.size * sizeof(int));
    for(int i = 0; i < a.size; ++i){
      a.array[i] = array->array[i];
    }
  }else{
    a.array = NULL;
  }
  return a;
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
  assert(array != NULL);
  if(array->size == 0){
    return false;
  }
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
    int const* pa = a;
    int const* pb = b;
    return cmp(*pa, *pb);
  };
  qsort(array->array, array->size, sizeof(int), qsort_cmp);
}

void int_array_sort_less(int_array* array){
  int_array_sort(array, int_compare);
}

void int_array_sort_less_bounded(int_array* array, int_array* tmp){
  memset(tmp->array, 0, tmp->size * sizeof(int));
  for(int i = 0; i < array->size; ++i){
    assert(array->array[i] < tmp->size);
    tmp->array[array->array[i]] += 1;
  }
  int cur = 0;
  for(int i = 0; i < tmp->size; ++i){
    for(int j = 0; j < tmp->array[i]; ++j){
      array->array[cur] = i;
      cur += 1;
    }
  }
}

bool int_array_unsorted_compare_bounded(int_array* a, int_array* b, int_array* tmp){
  assert(a != NULL && b != NULL);
  assert(a->size == b->size);
  memset(tmp->array, 0, tmp->size * sizeof(int));
  for(int i = 0; i < a->size; ++i){
    assert(a->array[i] < tmp->size);
    assert(b->array[i] < tmp->size);
    tmp->array[a->array[i]] += 1;
    tmp->array[b->array[i]] -= 1;
  }
  for(int i = 0; i < tmp->size; ++i){
    if(tmp->array[i] != 0){
      return false;
    }
  }
  return true;
}

unsigned int_array_hash_bounded(int_array* a, int_array* tmp){
  memset(tmp->array, 0, tmp->size * sizeof(int));
  for(int i = 0; i < a->size; ++i){
    assert(a->array[i] < tmp->size);
    tmp->array[a->array[i]] += 1;
  }
  unsigned h = 0;
  for(int i = 0; i < tmp->size; ++i){
    if(tmp->array[i] != 0){
      h = hash_combine(hash_combine(h, i), (unsigned) tmp->array[i]);
    }
  }
  return h;
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

int_array trivial_isomorphism(int size){
  int_array iso = int_array_new(size);
  for(int i = 0; i < size; ++i){
    iso.array[i] = i;
  }
  return iso;
}

int_array random_isomorphism(int size){
  int_array iso = trivial_isomorphism(size);
  for(int i = 0; i < size; ++i){
    int j = i + rand()% (size-i);
    SWAP(int, iso.array[i], iso.array[j]);
  }
  return iso;
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

void int_array_array_free(int_array_array* array){
  for(int i = 0; i < array->size; ++i){
    int_array_free(&array->array[i]);
  }
  free(array->array);
}

int_array_array int_array_array_copy(int_array_array* array){
  int_array_array a;
  a.size = array->size;
  a.bufferSize = a.size;
  if(a.size != 0){
    a.array = malloc(a.size * sizeof(int_array));
    for(int i = 0; i < a.size; ++i){
      a.array[i] = int_array_copy(&array->array[i]);
    }
  }else{
    a.array = NULL;
  }
  return a;
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

int int_array_back(int_array* array){
  assert(array->size > 0);
  return array->array[array->size - 1];
}

void int_array_remove_back(int_array* array){
  assert(array->size > 0);
  array->size -= 1;
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
