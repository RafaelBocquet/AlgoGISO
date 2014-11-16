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
bool test_isomorphism(const graph* a, const graph* b, int_array* iso){
  assert(a != NULL);
  assert(b != NULL);
  assert(iso != NULL);
  assert(a->size == b->size);
  assert(a->size == iso->size);

  for(int i = 0; i < a->size; ++i){
    if(a->array[i].size != b->array[iso->array[i]].size){
      return false;
    }
    
    for(int j = 0; j < a->array[i].size; ++j){
      if(!int_array_binary_search(&b->array[iso->array[i]], iso->array[a->array[i].array[j]])){
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
  if(a->size != b->size){
    return int_array_empty();
  }
  
  int_array iso = trivial_isomorphism(a->size);
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
  if(a->size != b->size){
    return int_array_empty();
  }

  int_array iso = trivial_isomorphism(a->size);

  bool backtrack(int i){
    // printf("%d %d %d\n", i, iso.array[0], a->size);
    if(i == a->size){
      return true;
    }else{
      /*
       * Remaining vertex in b are vertex iso[i..n-1]
       */
      for(int j = i; j < a->size; ++j){
        SWAP(int, iso.array[i], iso.array[j]);
        // Need to test array from iso[i]
        if(a->array[i].size == b->array[iso.array[i]].size){
          bool valid = true;
          for(int k = 0; k < a->array[i].size; ++k){
            if(a->array[i].array[k] <= i && !int_array_binary_search(&b->array[iso.array[i]], iso.array[a->array[i].array[k]])){
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
  
  if(a->size != b->size){
    return int_array_empty();
  }
  for(int i = 0; i < pa.size; ++i){
    if(pa.array[i].size != pb.array[i].size){
      return int_array_empty();
    }
  }

  int_array I = trivial_isomorphism(pa.size);
  int I_cmp(int a, int b){
    return pa.array[a].size - pa.array[b].size;
  }
  int_array_sort(&I, I_cmp);

  int_array iso = trivial_isomorphism(a->size);
  bool done[a->size];
  for(int i = 0; i < a->size; ++i){
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
        
        if(a->array[ai].size == b->array[aj].size){
          bool valid = true;
          for(int l = 0; l < a->array[ai].size; ++l){
            if(done[a->array[ai].array[l]] && !int_array_binary_search(&b->array[aj], iso.array[a->array[i].array[l]])){
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
  if(a->size != b->size){
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
 * Gets the stable partition.
 * Can be improved with bloom filters instead of signatures (TODO : stable_partition_fast)
 */

typedef enum refine_partition_result {
  REFINE_PARTITION_REFINE,
  REFINE_PARTITION_NOP,
  REFINE_PARTITION_FAIL
} refine_partition_result;

refine_partition_result refine_partition(graph* a, graph* b, partition* pa, partition* pb){
  assert(a != NULL && b != NULL);
  assert(pa != NULL && pb != NULL);
  assert(a->size == b->size);
  assert(pa->elements.size == a->size);
  assert(pb->elements.size == b->size);

  bool refined = false;

  partition npa = partition_new(a->size);
  partition npb = partition_new(a->size);

  for(int i = 0; i < pa->partition.size; ++i){
    // TODO : check this condition
    if(pa->partition.array[i].size != pb->partition.array[i].size){
      return REFINE_PARTITION_FAIL;
    }
    
    if(pa->partition.array[i].size > 0){
      if(pa->partition.array[i].size == 1){
        // No refinement possible
        int cls = partition_new_class(&npa);
        partition_new_class(&npb);
        partition_set_class(&npa, pa->partition.array[i].array[0], cls);
        partition_set_class(&npb, pb->partition.array[i].array[0], cls);
      }else{
        // Signatures
        int_array_array sigsa = int_array_array_new(pa->partition.array[i].size);
        int_array_array sigsb = int_array_array_new(pa->partition.array[i].size);
        for(int j = 0; j < pa->partition.array[i].size; ++j){
          int ai = pa->partition.array[i].array[j];
          int bi = pb->partition.array[i].array[j];
          assert(a->array[ai].size == b->array[bi].size);
          
          sigsa.array[j] = int_array_new(a->array[ai].size);
          sigsb.array[j] = int_array_new(b->array[bi].size);
          for(int k = 0; k < a->array[ai].size; ++k){
            sigsa.array[j].array[k] = pa->elements.array[a->array[ai].array[k]];
            sigsb.array[j].array[k] = pb->elements.array[b->array[bi].array[k]];
          }
          int_array_sort_less(&sigsa.array[j]);
          int_array_sort_less(&sigsb.array[j]);
        }

        // To partition the signatures, while preserving knowledge of which nodes these are the signatures
        
        int_array I = trivial_isomorphism(sigsa.size);
        int I_cmp(int a, int b){
          return int_array_compare(&sigsa.array[a], &sigsa.array[b]);
        }
        int_array_sort(&I, I_cmp);
        
        int_array J = trivial_isomorphism(sigsb.size);
        int J_cmp(int a, int b){
          return int_array_compare(&sigsb.array[a], &sigsb.array[b]);
        }
        int_array_sort(&J, J_cmp);

        // Partition of signatures
        int j = 0;
        while(j != sigsa.size){
          if(int_array_compare(&sigsa.array[I.array[j]], &sigsb.array[J.array[j]]) != 0){
            return REFINE_PARTITION_FAIL;
          }
          int ka = j + 1, kb = j + 1;
          while(ka < sigsa.size && int_array_compare(&sigsa.array[I.array[j]], &sigsa.array[I.array[ka]]) == 0){
            ka += 1;
          }
          while(kb < sigsa.size && int_array_compare(&sigsb.array[J.array[j]], &sigsb.array[J.array[kb]]) == 0){
            kb += 1;
          }
          if(ka != kb){
            return REFINE_PARTITION_FAIL;
          }
          int cls = partition_new_class(&npa);
          partition_new_class(&npb);
          while(j != ka){
            partition_set_class(&npa, pa->partition.array[i].array[I.array[j]], cls);
            partition_set_class(&npb, pb->partition.array[i].array[J.array[j]], cls);
            j += 1;
          }
          if(j != sigsa.size){
            refined = true;
          }
        }
        int_array_array_free(&sigsa);
        int_array_array_free(&sigsb);
        int_array_free(&I);
        int_array_free(&J);
      }
    }
  }

  if(refined){
    partition_cleanup(&npa);
    partition_cleanup(&npb);
    partition_free(pa);
    partition_free(pb);
    *pa = npa;
    *pb = npb;
    return REFINE_PARTITION_REFINE;
  }else{
    partition_free(&npa);
    partition_free(&npb);
    return REFINE_PARTITION_NOP;
  }
}

bool stable_partition(graph* a, graph* b, partition* pa, partition* pb){
  assert(a != NULL && b != NULL);
  assert(pa != NULL && pb != NULL);
  refine_partition_result r;
  while((r = refine_partition(a, b, pa, pb)) == REFINE_PARTITION_REFINE){ }
  return r != REFINE_PARTITION_FAIL;
}

int_array graph_isomorphism_WL(graph* a, graph* b){
  assert(a != NULL);
  assert(b != NULL);
  if(a->size != b->size){
    return int_array_empty();
  }
  
  bool backtrack(partition* pa, partition* pb, int depth){
    printf("Backtrack depth %d\n", depth);
    if(!stable_partition(a, b, pa, pb)){
      return false;
    }
    // remove empty classes. Keeps the ordering of classes in pa and pb -> valid
    partition_cleanup(pa);
    partition_cleanup(pb);

    // TODO : better choice of i
    int i = 0;
    while(i < a->size && pa->partition.array[i].size == 1){
      i += 1;
    }
    // 1 element / class : isomorphism
    if(i == a->size){
      return true;
    }
    
    for(int j = 0; j < pb->partition.array[i].size; ++j){
      partition opa = partition_copy(pa);
      partition opb = partition_copy(pb);
      int cls = partition_new_class(&opa);
      partition_new_class(&opb);
      int ai = int_array_back(&opa.partition.array[i]);
      int_array_remove_back(&opa.partition.array[i]);
      int aj = opb.partition.array[i].array[j];
      opb.partition.array[i].array[j] = int_array_back(&opb.partition.array[i]);
      int_array_remove_back(&opb.partition.array[i]);
      partition_set_class(&opa, ai, cls);
      partition_set_class(&opb, aj, cls);
      if(backtrack(&opa, &opb, depth+1)){
        SWAP(partition, opa, *pa);
        SWAP(partition, opb, *pb);
        partition_free(&opa);
        partition_free(&opb);
        return true;
      }else{
        partition_free(&opa);
        partition_free(&opb);
      }
    }

    return false;
  }

  partition pa = graph_degree_partition(a);
  partition pb = graph_degree_partition(b);

  if(backtrack(&pa, &pb, 0)){
    partition_cleanup(&pa);
    partition_cleanup(&pb);
    int_array iso = int_array_new(a->size);
    for(int i = 0; i < a->size; ++i){
      iso.array[pa.partition.array[i].array[0]] = pb.partition.array[i].array[0];
    }
    return iso;
  }else{
    return int_array_empty();
  }
}

int main(int argc __attribute__((unused)), char** argv __attribute__((unused))){
  // Entrée
  printf("Read graph 1\n");
  graph a = graph_read_matrix();
  printf("Read graph 2\n");
  graph b = graph_read_matrix();
  // Appel de l'algorithme
  int_array iso;
  if((iso = graph_isomorphism_WL(&a, &b)).size != 0){
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
