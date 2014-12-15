#ifndef ALGO_GISO_GRAPH_H
#define ALGO_GISO_GRAPH_H

#include "stdlib.h"
#include "stdio.h"

#include "array.h"
#include "partition.h"
#include "util.h"
#include "wl_partition.h"

/*
 * Choix effectués:
 * Représentation des graphes sous forme de listes d'adjacences.
 * Ces listes d'adjacences sont en fait des tableaux, triés pour pouvoir effectuer des recherches en temps logarithmique
 */

typedef int_array_array graph;

graph graph_random(int size, int nedge);
graph graph_read();
graph graph_read_matrix();
void graph_write_matrix(graph* g);
void graph_free(graph* g);
partition graph_degree_partition(graph* g);
// empty partition if invalid
wl_partition wl_graph_degree_partition(graph* g[2]);
graph graph_reverse(graph* g);
graph graph_apply_isomorphism(graph* g, int_array* iso);

#endif
