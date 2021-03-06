#include "graph.h"


// O(size*size) memory, we can do O(size) with a binary tree (for instance)
graph graph_random(int size, int nedge){
  graph g = int_array_array_new(size);
  int_array rnd = trivial_isomorphism(size*size);
  for(int i = 0; i < nedge; ++i){
    int j = i + rand() % (size*size-i);
    SWAP(int, rnd.array[i], rnd.array[j]);
    int_array_append(&g.array[rnd.array[i] / size], rnd.array[i] % size);
  }
  int_array_free(&rnd);
  for(int i = 0; i < g.size; ++i){
    int_array_sort_less(&g.array[i]);
  }
  return g;
}

graph graph_read(){
  int size;
  scanf("%d\n", &size);
  graph g = int_array_array_new(size);
  for(int i = 0; i < size; ++i) {
    int n; scanf("%d", &n);
    for(int j = 0; j < n; ++j){
      int k; scanf("%d", &k);
      int_array_append(&g.array[i], k);
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

void graph_write_matrix(graph* g){
  printf("%d\n", g->size);
  for(int i = 0; i < g->size; ++i){
    for(int j = 0; j < g->size; ++j){
      printf("%c", int_array_binary_search(&g->array[i], j)?'1':'0');
    }
    printf("\n");
  }
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
  partition a = partition_new_with_classes(g->size, g->size + 1);
  for(int i = 0; i < g->size; ++i){
    partition_set_class(&a, i, g->array[i].size);
  }
  // partition_cleanup(&a);
  return a;
}

wl_partition wl_graph_degree_partition(graph* g[2]){
  assert(g[0] != NULL && g[1] != NULL);
  assert(g[0]->size == g[1]->size);
  wl_partition p = wl_partition_new_with_classes(g[0]->size, g[0]->size + 1);
  TWICE(j) for(int i = 0; i < g[j]->size; ++i){
    wl_partition_set_class_single(&p, g[j]->array[i].size, j, i);
    for(int k = 0; k < g[j]->array[i].size; ++k){
      int a = g[j]->array[i].array[k];
      p.elements_hash[j].array[i] += wl_hash_f(g[j]->array[a].size);
      p.elements_hash[j].array[a] += int_rotate(wl_hash_f(g[j]->array[i].size));
    }
  }
  /* if(!wl_partition_cleanup(&p)){ */
  /*   wl_partition_free(&p); */
  /*   p = wl_partition_empty(); */
  /* }else{ */
  int_set_free(&p.update_queue);
  p.update_queue = int_set_range(0, p.partition.size-1);
  /* } */
  return p;
}

graph graph_reverse(graph* g){
  assert(g != NULL);
  graph h = int_array_array_new(g->size);
  for(int i = 0; i < g->size; ++i){
    for(int j = 0; j < g->array[i].size; ++j){
      int_array_append(&h.array[g->array[i].array[j]], i);
    }
  }
  for(int i = 0; i < h.size; ++i){
    int_array_sort_less(&h.array[i]);
  }
  return h;
}

graph graph_apply_isomorphism(graph* g, int_array* iso){
  assert(g != NULL);
  assert(iso != NULL);
  assert(g->size == iso->size);
  graph h = int_array_array_new(g->size);
  for(int i = 0; i < g->size; ++i){
    for(int j = 0; j < g->array[i].size; ++j){
      int_array_append(&h.array[iso->array[i]], iso->array[g->array[i].array[j]]);
    }
  }
  for(int i = 0; i < h.size; ++i){
    int_array_sort_less(&h.array[i]);
  }
  return h;
}

