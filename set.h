#ifndef ALGO_GISO_RB_SET_H
#define ALGO_GISO_RB_SET_H

#include "stdlib.h"
#include "stdbool.h"

typedef struct set_node {
  int value;
  struct set_node *l, *r;
} set_node;

typedef set_node* int_set;

int_set int_set_empty();
void int_set_free(int_set* s);
int_set int_set_range(int a, int b);
int_set int_set_copy(int_set* s);
void int_set_print(int_set* s);
bool int_set_is_empty(int_set* s);
bool int_set_insert(int_set* s, int v);
int int_set_delete(int_set* s);
void int_set_map_monotonous(int_set* s, int(*f)(int));

#endif
