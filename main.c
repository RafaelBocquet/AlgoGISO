#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "math.h"
#include "assert.h"

#include "array.h"
#include "util.h"
#include "graph.h"
#include "partition.h"

/*
 * Test if iso is an isomorphism between graphs a and b
 */
bool test_isomorphism(graph* a, graph* b, int_array* iso){
  assert(a != NULL);
  assert(b != NULL);
  assert(iso != NULL);
  assert(a->graphSize == b->graphSize);
  assert(a->graphSize == iso->size);

  for(int i = 0; i < a->graphSize; ++i){
    if(a->vertices[i].size != b->vertices[iso->array[i]].size){
      return false;
    }
    
    for(int j = 0; j < a->vertices[i].size; ++j){
      if(!int_array_binary_search(&b->vertices[iso->array[i]], iso->array[a->vertices[i].array[j]])){
        return false;
      }
    }
  }
  return true;
}

/*
 * Returns false if iso is the last isomorphism
 * Returns true otherwise
 *
 * Used to enumerate all isomorphisms
 */
bool next_isomorphism(int_array* iso){
  assert(iso != NULL);
  int k = iso->size - 2;
  while(k >= 0 && iso->array[k] > iso->array[k+1]){
    k -= 1;
  }
  if(k < 0){
    return false;
  }

  int l = iso->size - 1;
  while(iso->array[k] > iso->array[l]){
    l -= 1;
  }

  SWAP(int, iso->array[k], iso->array[k]);

  k += 1;
  l = iso->size - 1;
  while(k < l){
    SWAP(int, iso->array[k], iso->array[l]);
    k += 1;
    l -= 1;
  }

  return true;
}

int_array trivial_isomorphism(int size){
  int_array iso = int_array_new(size);
  for(int i = 0; i < size; ++i){
    iso.array[i] = i;
  }
  return iso;
}

/*
 * graph_isomorphism_* functions
 * Returns int_array_empty() if graphs are not isomorphic
 * Returns the isomorphism otherwise
 */

// Iterates over all isormorphisms

int_array graph_isomorphism_1(graph* a, graph* b){
  assert(a != NULL);
  assert(b != NULL);
  if(a->graphSize != b->graphSize){
    return int_array_empty();
  }
  
  int_array iso = trivial_isomorphism(a->graphSize);
  while(true){
    if(test_isomorphism(a, b, &iso)){
      return iso;
    }
    if(!next_isomorphism(&iso)){
      break;
    }
  }
  
  int_array_free(&iso);
  return int_array_empty();
}

// Iterates over all isomorphisms, with backtracing

int_array graph_isomorphism_2(graph* a, graph* b){
  assert(a != NULL);
  assert(b != NULL);
  if(a->graphSize != b->graphSize){
    return int_array_empty();
  }

  int_array iso = trivial_isomorphism(a->graphSize);
 
  bool backtrack(int i){
    if(i == a->graphSize){
      return true;
    }else{
      /*
       * Remaining vertex in b are vertex iso[i..n-1]
       */
      for(int j = i; j < a->graphSize; ++j){
        SWAP(int, iso.array[i], iso.array[j]);
        // Need to test vertices from iso[i]
        if(a->vertices[i].size == b->vertices[iso.array[j]].size){
          bool valid = true;
          for(int k = 0; k < a->vertices[i].size; ++k){
            if(a->vertices[i].array[k] <= i && !int_array_binary_search(&b->vertices[iso.array[i]], iso.array[a->vertices[i].array[k]])){
              valid = false;
            }
          }
          if(valid && backtrack(i+1)){
            return true;
          }
        }
      }
      return false;
    }
  }
 
  if(backtrack(0)){
    return iso;
  }else{
    int_array_free(&iso);
    return int_array_empty();
  }
}

// Iterates over all isomorphisms, with backtracing, with pruning using a partition

int_array graph_isomorphism_partition(graph* a, graph* b, partition* a_part, partition* b_part){
  assert(a != NULL && b != NULL);
  assert(a_part != NULL && b_part != NULL);
  int_array_array pa = a_part->partition;
  int_array_array pb = b_part->partition;
  assert(pa.size == pb.size);
  
  if(a->graphSize != b->graphSize){
    return int_array_empty();
  }
  for(int i = 0; i < pa.size; ++i){
    if(pa.array[i].size != pb.array[i].size){
      return int_array_empty();
    }
  }

  int_array I = int_array_new(pa.size);
  int I_cmp(int a, int b){
    return pa.array[b].size - pa.array[a].size;
  }
  int_array_sort(&I, I_cmp);

  int_array iso = trivial_isomorphism(a->graphSize);
  bool done[a->graphSize];
  for(int i = 0; i < a->graphSize; ++i){
    done[i] = false;
  }
        
  bool backtrack(int i, int j){
    if(i == I.size){
      return true;
    }else if(j == pa.array[I.array[i]].size){
      return backtrack(i+1, 0);
    }else{
      int ai = pa.array[I.array[i]].array[j];
      done[ai] = true;
      
      for(int k = j; k < pa.array[I.array[i]].size; ++k){
        SWAP(int, pb.array[I.array[i]].array[j], pb.array[I.array[i]].array[k]);
        
        int aj = pb.array[I.array[i]].array[j];
        iso.array[ai] = aj;
        
        if(a->vertices[ai].size == b->vertices[aj].size){
          bool valid = true;
          for(int l = 0; l < a->vertices[ai].size; ++l){
            if(done[a->vertices[ai].array[l]] && !int_array_binary_search(&b->vertices[aj], iso.array[a->vertices[i].array[l]])){
              valid = false;
            }
          }
          if(valid && backtrack(i, j+1)){
            return true;
          }
        }
      }
      
      done[ai] = false;
      return false;
    }
  }

  if(backtrack(0, 0)){
    return iso;
  }else{
    int_array_free(&iso);
    return int_array_empty();
  }
}

int_array graph_isomorphism_degree_partition(graph* a, graph* b){
  assert(a != NULL);
  assert(b != NULL);
  if(a->graphSize != b->graphSize){
    return int_array_empty();
  }
  partition pa  = graph_degree_partition(a);
  partition pb  = graph_degree_partition(b);
  int_array iso = graph_isomorphism_partition(a, b, &pa, &pb);
  partition_free(&pa);
  partition_free(&pb);
  return iso;
}

/*
 * Not the best stable partition (use of a bloom filter with possible collisions)
 */

void stable_partition(graph* g, partition* p){
  assert(g != NULL);
}

int main(int argc, char** argv){
  // Entrée
  printf("Read graph 1\n");
  graph a = graph_read();
  printf("Read graph 2\n");
  graph b = graph_read();
  // Appel de l'algorithme
  int_array iso;
  if((iso = graph_isomorphism_degree_partition(&a, &b)).size != 0){
    printf("oui\n");
    int_array_free(&iso);
  }else{
    printf("non\n");
  }
  // Cleanup
  graph_free(&a);
  graph_free(&b);
  return 0;
}
