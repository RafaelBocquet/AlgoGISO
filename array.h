#ifndef ALGO_GISO_ARRAY_H
#define ALGO_GISO_ARRAY_H

#include "string.h"
#include "stdlib.h"
#include "stdbool.h"
#include "assert.h"
#include "util.h"

// int_array

typedef struct int_array{
  int  size;
  int  bufferSize;
  int* array;
} int_array;

int_array int_array_empty();
int_array int_array_new(int size);
void int_array_free(int_array *array);
int_array int_array_copy(int_array* array);
void int_array_append(int_array* array, int value);
bool int_array_binary_search(int_array* array, int value);
void int_array_sort(int_array* array, int (*cmp)(int, int));
void int_array_sort_less(int_array* array);
void int_array_sort_less_bounded(int_array* array, int_array* tmp);
bool int_array_unsorted_compare_bounded(int_array* a, int_array* b, int_array* tmp);
int int_array_compare(int_array* a, int_array* b);
int int_array_back(int_array* array);
void int_array_remove_back(int_array* array);


int_array trivial_isomorphism(int size);
int_array random_isomorphism(int size);

// int_array_array

typedef struct int_array_array{
  int        size;
  int        bufferSize;
  int_array* array;
} int_array_array;

int_array_array int_array_array_empty();
int_array_array int_array_array_new(int size);
void int_array_array_free(int_array_array* array);
int_array_array int_array_array_copy(int_array_array* array);
void int_array_array_append(int_array_array* array, int_array value);
void int_array_array_sort(int_array_array* array, int (*cmp)(int_array*, int_array*));
void int_array_array_sort_less(int_array_array* array);

// int_array_pair_array

typedef struct int_array_pair_array{
  int       size;
  int       bufferSize;
  int_array (*array)[2];
} int_array_pair_array;

int_array_pair_array int_array_pair_array_empty();
int_array_pair_array int_array_pair_array_new(int size);
void int_array_pair_array_free(int_array_pair_array* array);
int_array_pair_array int_array_pair_array_copy(int_array_pair_array* array);
void int_array_pair_array_append(int_array_pair_array* array, int_array value[2]);

#endif
