#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "math.h"
#include "assert.h"
#include "inttypes.h"
#include "time.h"

#include "array.h"
#include "util.h"
#include "set.h"
#include "graph.h"
#include "partition.h"
#include "wl_partition.h"

/*
 * Algorithm to test whether iso is a valid isomorphism between graphs a and b
 * Complexity : O(E)
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
 * Returns false if iso is the last isomorphism (n (n-1) (n-2) ... 3 2 1)
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

  SWAP(int, iso->array[k], iso->array[l]);

  k += 1;
  l = iso->size - 1;
  while(k < l){
    SWAP(int, iso->array[k], iso->array[l]);
    k += 1;
    l -= 1;
  }

  return true;
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
    if(i == a->size){
      return test_isomorphism(a, b, &iso);
    }else{
      /*
       * Remaining vertex in b are vertex iso[i..n-1]
       */
      for(int j = i; j < a->size; ++j){
        SWAP(int, iso.array[i], iso.array[j]);
        // Need to test new edges in the subgraph with vertices in [0..i]
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

        SWAP(int, iso.array[i], iso.array[j]);
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

// Iterates over all isomorphisms, with backtracing and pruning using a partition
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
        
        SWAP(int, pb.array[I.array[i]].array[j], pb.array[I.array[i]].array[k]);
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

// Iterates over all isomorphisms, with backtracing and pruning using a partition
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
 * update_neighbours
 *
 * Update neighbours in a partition when it is refined
 */

void update_neighbours(graph* g[2], graph* rg[2], wl_partition* p, int pi){
  int psize = p->partition.array[pi][0].size;
  
  for(int k = 0; k < psize; ++k){
    int k_[2] = { p->partition.array[pi][0].array[k],
                  p->partition.array[pi][1].array[k] };
    int_array* a_[2] = { &g[0]->array[k_[0]],
                         &g[1]->array[k_[1]] };
    int_array* ra_[2] = { &rg[0]->array[k_[0]],
                          &rg[1]->array[k_[1]] };
    // Mark neighbouring classes
    for(int m = 0; m < a_[0]->size; ++m){
      int_set_insert(&p->update_queue, p->elements[0].array[a_[0]->array[m]]);
    }
    for(int m = 0; m < ra_[0]->size; ++m){
      int_set_insert(&p->update_queue, p->elements[0].array[ra_[0]->array[m]]);
    }
    // Update hashes
    TWICE(j){
      for(int m = 0; m < a_[j]->size; ++m){
        p->elements_hash[j].array[a_[j]->array[m]] += int_rotate(wl_hash_f(p->elements[j].array[k_[j]])) - int_rotate(wl_hash_f(pi));
      }
    }
    TWICE(j) for(int m = 0; m < ra_[j]->size; ++m){
      p->elements_hash[j].array[ra_[j]->array[m]] += wl_hash_f(p->elements[j].array[k_[j]]) - wl_hash_f(pi);
    }
  }
}

/*
 * stable_partition
 *
 * Finds the maximum refinement of a partition of g
 * Returns false if p is an invalid partition, and true otherwise
 * Updates p with the new partition
 *
 * For a given vertex, we compute its signature in the graph g : the multiset of the partition classes of its neighbours
 * A hash of the multiset is actually computed instead of the multiset to avoid array sorting
 *
 * We inspect all classes of the given partition, spliting some classes into new classes until the partition can't be refined.
 * For each class, we split the class if it is possible
 * When a class is split, we remember we have to check its neighbour classes
 * 
 */

bool stable_partition(graph* g[2], graph* rg[2], wl_partition* p){
  TWICE(i) assert(g[i] != NULL);
  TWICE(i) assert(rg[i] != NULL);
  assert(p != NULL);
  assert(g[0]->size == g[1]->size);
  assert(rg[0]->size == rg[1]->size);
  assert(g[0]->size == rg[0]->size);
  TWICE(i) assert(g[i]->size == p->elements[i].size);

  // Compute the signature of vertex j in graph g when g is g[i] or rg[i]
  int_array signature(graph* g, int i, int j){
    int_array sig = int_array_new(g->array[j].size);
    for(int k = 0; k < g->array[j].size; ++k){
      sig.array[k] = p->elements[i].array[g->array[j].array[k]];
    }
    return sig;
  }

  while(!int_set_is_empty(&p->update_queue)){
    int i = int_set_delete(&p->update_queue);
    int psize = p->partition.array[i][0].size;
    
    if(p->partition.array[i][0].size != p->partition.array[i][1].size){
      return false;
    }
    
    if(psize > 0){
      if(psize == 1){
        // No refinement possible, still check if the partition is valid !
        int a[2];
        TWICE(j) a[j] = p->partition.array[i][j].array[0];
        if(g[0]->array[a[0]].size != g[1]->array[a[1]].size
           || rg[0]->array[a[0]].size != rg[1]->array[a[1]].size){
          return false;
        }
        if(p->elements_hash[0].array[a[0]] != p->elements_hash[1].array[a[1]]){
          return false;
        }
        int_array sig[2]; TWICE(j) {
          sig[j] = signature(g[j], j, a[j]);
          int_array_sort_less(&sig[j]);
        }
        int_array rsig[2]; TWICE(j) {
          rsig[j] = signature(rg[j], j, a[j]);
          int_array_sort_less(&rsig[j]);
        }
        void local_free(){
          TWICE(j) {
            int_array_free(&sig[j]);
            int_array_free(&rsig[j]);
          }
        }
        if(int_array_compare(&sig[0], &sig[1]) != 0
           || int_array_compare(&rsig[0], &rsig[1]) != 0){
          local_free();
          return false;
        }
        local_free();
        // Partition is valid from this node
      }else{
        // Partitioning items with different signatures

        // if the partition is valid, sorting signatures of the partition's vertex in the two graph should give the same result
        // we sort these using indices to know which elements should go in the same partition class
        int_array I[2];
        TWICE(k) {
          I[k] = trivial_isomorphism(psize);
          int I_cmp(int a, int b){
            return int_compare(p->elements_hash[k].array[p->partition.array[i][k].array[a]],
			       p->elements_hash[k].array[p->partition.array[i][k].array[b]]);
          }
          int_array_sort(&I[k], I_cmp);
        }
        
        void local_free(){
          TWICE(j) int_array_free(&I[j]);
        }

        // If a refinement is possible
        if(p->elements_hash[0].array[p->partition.array[i][0].array[I[0].array[0]]] !=
           p->elements_hash[0].array[p->partition.array[i][0].array[I[0].array[psize-1]]] ||
	   p->elements_hash[1].array[p->partition.array[i][1].array[I[1].array[0]]] !=
           p->elements_hash[1].array[p->partition.array[i][1].array[I[1].array[psize-1]]] ||
	   p->elements_hash[0].array[p->partition.array[i][0].array[I[0].array[0]]] !=
           p->elements_hash[1].array[p->partition.array[i][1].array[I[1].array[0]]]){
          int j = 0;
          while(j != psize){
            if(p->elements_hash[0].array[p->partition.array[i][0].array[I[0].array[j]]] !=
               p->elements_hash[1].array[p->partition.array[i][1].array[I[1].array[j]]]){
              local_free();
              return false;
            }
            int k[2]; TWICE(l) k[l] = j + 1;
            TWICE(l) while(k[l] < psize &&
                           p->elements_hash[l].array[p->partition.array[i][l].array[I[l].array[j]]] ==
                           p->elements_hash[l].array[p->partition.array[i][l].array[I[l].array[k[l]]]]){
              k[l] += 1;
            }
            if(k[0] != k[1]){
              local_free();
              return false;
            }
            int cls = wl_partition_new_class(p);

            while(j != k[0]){
              int el[2]; TWICE(l) el[l] = p->partition.array[i][l].array[I[l].array[j]];
              wl_partition_set_class(p, cls, el);
              j += 1;
            }
          }
          update_neighbours(g, rg, p, i);
	  TWICE(j) {
            int_array_free(&p->partition.array[i][j]);
            p->partition.array[i][j] = int_array_empty();
          }
        }
        local_free();
      }
    }
  }
  return true;
}

/*
 * graph_isomorphism_WL
 * Weisfeiler-Lehman algorithm
 */

int_array graph_isomorphism_WL(graph* g[2]){
  TWICE(i) assert(g[i] != NULL);
  if(g[0]->size != g[1]->size){
    return int_array_empty();
  }

  graph rg_[2]; TWICE(i) rg_[i] = graph_reverse(g[i]);
  graph* rg[2] = { &rg_[0], &rg_[1] };
  
  bool backtrack(wl_partition* p, int depth){
    if(!stable_partition(g, rg, p)){
      return false;
    }
    
    // TODO : better choice of i ?
    // Smallest / largest class ?
    int i = 0;
    while(i < p->partition.size && p->partition.array[i][0].size <= 1){
      i += 1;
    }
    
    // 1 element / class : isomorphism
    if(i == p->partition.size){
      return true;
    }
    
    for(int j = 0; j < p->partition.array[i][0].size; ++j){
      wl_partition p_ = wl_partition_copy(p);

      int cls = wl_partition_new_class(&p_);
      int a[2];

      a[0] = int_array_back(&p_.partition.array[i][0]);
      int_array_remove_back(&p_.partition.array[i][0]);

      a[1] = p_.partition.array[i][1].array[j];
      p_.partition.array[i][1].array[j] = int_array_back(&p_.partition.array[i][1]);
      int_array_remove_back(&p_.partition.array[i][1]);

      wl_partition_set_class(&p_, cls, a);

      // For all neighbours of the old class
      for(int k = 0; k < p->partition.array[i][0].size; ++k){
        int k_[2] = { p->partition.array[i][0].array[k],
                      p->partition.array[i][1].array[k] };
        for(int m = 0; m < g[0]->array[k_[0]].size; ++m){
          int_set_insert(&p_.update_queue, p->elements[0].array[g[0]->array[k_[0]].array[m]]);
        }
        for(int m = 0; m < rg[0]->array[k_[0]].size; ++m){
          int_set_insert(&p_.update_queue, p->elements[0].array[rg[0]->array[k_[0]].array[m]]);
        }
      }
      // For all neighbours of the new class
      TWICE(j) for(int m = 0; m < g[j]->array[a[j]].size; ++m){
        p_.elements_hash[j].array[g[j]->array[a[j]].array[m]] += int_rotate(wl_hash_f(p_.elements[j].array[a[j]])) - int_rotate(wl_hash_f(i));
      }
      TWICE(j) for(int k = 0; k < rg[j]->array[a[j]].size; ++k){
        p_.elements_hash[j].array[rg[j]->array[a[j]].array[k]] += wl_hash_f(p_.elements[j].array[a[j]]) - wl_hash_f(i);
      }
      
      if(backtrack(&p_, depth+1)){
        SWAP(wl_partition, p_, *p)
        wl_partition_free(&p_);
        return true;
      }else{
        wl_partition_free(&p_);
      }
    }

    return false;
  }

  wl_partition p = wl_graph_degree_partition(g);
  
  void local_free(){
    wl_partition_free(&p);
    TWICE(i) graph_free(rg[i]);
  }

  if(backtrack(&p, 0)){
    int_array iso = int_array_new(g[0]->size);
    for(int i = 0; i < p.partition.size; ++i) if(p.partition.array[i][0].size != 0){
      iso.array[p.partition.array[i][0].array[0]] = p.partition.array[i][1].array[0];
    }
    local_free();
    return iso;
  }else{
    local_free();
    return int_array_empty();
  }
}

int main(int argc __attribute__((unused)), char** argv __attribute__((unused))){
  srand(time(NULL));
  // Entrée
  graph a = graph_read_matrix();
  graph b = graph_read_matrix();
  // Appel de l'algorithme
  int_array iso;
  graph* g[2] = { &a, &b };

  if((iso = graph_isomorphism_WL(g)).size != 0){
    printf("oui\n");
    for(int i = 0; i < a.size; ++i){
      printf("%d ", iso.array[i]);
    } printf("\n");
    assert(test_isomorphism(&a, &b, &iso));
    int_array_free(&iso);
  }else{
    printf("non\n");
  }
  // Cleanup
  graph_free(&a);
  graph_free(&b);
  return 0;
}
