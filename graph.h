#ifndef ALGO_GISO_GRAPH_H
#define ALGO_GISO_GRAPH_H

#include "stdlib.h"
#include "array.h"
#include "partition.h"

/*
 * Choix effectués:
 * Représentation des graphes sous forme de listes d'adjacences.
 * Ces listes d'adjacences sont en fait des tableaux, triés pour pouvoir effectuer des recherches en temps logarithmique
 */

typedef int_array_array graph;

graph graph_read(){
  int size;
  scanf("%d", &size);
  graph g = int_array_array_new(size);
  int m; scanf("%d", &m);
  for(int j = 0; j < m; ++j){
    int a, b; scanf("%d%d", &a, &b);
    int_array_append(&g.array[a], b);
    if(a != b){
      int_array_append(&g.array[b], a);
    }
  }
  for(int i = 0; i < g.size; ++i){
    int_array_sort_less(&g.array[i]);
  }
  return g;
}

graph graph_read_matrix(){
  int size;
  scanf("%d", &size);
  graph g = int_array_array_new(size);
  for(int i = 0; i < size; ++i){
    for(int j = 0; j < size; ++j){
      char c = '\n'; while(c == '\n') scanf("%c", &c);
      if(c == '1'){
        int_array_append(&g.array[i], j);
      }
    }
  }
  for(int i = 0; i < g.size; ++i){
    int_array_sort_less(&g.array[i]);
  }
  return g;
}

void graph_free(graph* g){
  assert(g != NULL);
  for(int i = 0; i < g->size; ++i){
    int_array_free(&g->array[i]);
  }
  free(g->array);
}

partition graph_degree_partition(graph* g){
  assert(g != NULL);
  partition a = partition_new_with_classes(g->size, g->size);
  for(int i = 0; i < g->size; ++i){
    partition_set_class(&a, i, g->array[i].size);
  }
  //  partition_cleanup(&a);
  return a;
}

#endif
