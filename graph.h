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

typedef struct graph {
  int graphSize;
  int_array *vertices;
} graph;

graph graph_read(){
  graph g;
  scanf("%d", &g.graphSize);
  g.vertices = malloc(sizeof(int_array) * g.graphSize);
  for(int i = 0; i < g.graphSize; ++i){
    g.vertices[i] = int_array_empty();
  }
  int m; scanf("%d", &m);
  for(int j = 0; j < m; ++j){
    int a, b; scanf("%d%d", &a, &b);
    int_array_append(&g.vertices[a], b);
    int_array_append(&g.vertices[b], a);
  }
  for(int i = 0; i < g.graphSize; ++i){
    int_array_sort_less(&g.vertices[i]);
  }
  return g;
}

void graph_free(graph* g){
  assert(g != NULL);
  for(int i = 0; i < g->graphSize; ++i){
    int_array_free(&g->vertices[i]);
  }
  free(g->vertices);
}

partition graph_degree_partition(graph* g){
  assert(g != NULL);
  partition a = partition_new_with_classes(g->graphSize, g->graphSize);
  for(int i = 0; i < g->graphSize; ++i){
    partition_set_class(&a, i, g->vertices[i].size);
  }
  partition_cleanup(&a);
  return a;
}

#endif
