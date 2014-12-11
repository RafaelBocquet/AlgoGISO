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
    // printf("%d %d %d\n", i, iso.array[0], a->size);
    if(i == a->size){
      return test_isomorphism(a, b, &iso);
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

bool stable_partition(graph (*g)[2], graph (*rg)[2], wl_partition* p){
  TWICE(i) assert(g[i] != NULL);
  TWICE(i) assert(rg[i] != NULL);
  assert(p != NULL);
  assert(g[0]->size == g[1]->size);
  assert(rg[0]->size == rg[1]->size);
  assert(g[0]->size == rg[0]->size);
  TWICE(i) assert(g[i]->size == p->elements[i].size);

  int_array signature(graph* g, int i, int j){
    int_array sig = int_array_new(g->array[j].size);
    for(int k = 0; k < g->array[j].size; ++k){
      sig.array[k] = p->elements[i].array[g->array[j].array[k]];
    }
    return sig;
  }

  while(!int_set_is_empty(&p->update_queue)){
    int i = int_set_delete(&p->update_queue);
    
    if(p->partition.array[i][0].size != p->partition.array[i][1].size){
      //      local_free();
      return false;
    }
    
    if(p->partition.array[i][0].size > 0){
      if(p->partition.array[i][0].size == 1){
        /* // No refinement possible, still check if the partition is valid ! */
        /* int ai = pa->partition.array[i].array[0]; */
        /* int bi = pb->partition.array[i].array[0]; */
        /* if(a->array[ai].size != b->array[bi].size */
        /*    || ra->array[ai].size != rb->array[bi].size){ */
        /*   local_free(); */
        /*   return false; */
        /* } */
        /* int_array siga  = signature(a, pa, ai); */
        /* int_array sigb  = signature(b, pb, bi); */
        /* int_array sigra = signature(ra, pa, ai); */
        /* int_array sigrb = signature(rb, pb, bi); */
        /* int_array_sort_less(&siga); */
        /* int_array_sort_less(&sigb); */
        /* int_array_sort_less(&sigra); */
        /* int_array_sort_less(&sigrb); */
        /* if(int_array_compare(&siga, &sigb) != 0 */
        /*    || int_array_compare(&sigra, &sigrb) != 0){ */
        /*   local_free(); */
        /*   int_array_free(&siga); */
        /*   int_array_free(&sigb); */
        /*   int_array_free(&sigra); */
        /*   int_array_free(&sigrb); */
        /*   return false; */
        /* } */
        /* int_array_free(&siga); */
        /* int_array_free(&sigb); */
        /* int_array_free(&sigra); */
        /* int_array_free(&sigrb); */
        /* // Partition is valid from this node */
      }else{
        // Signatures
        int_array_array sigs[2]; TWICE(j) sigs[j] = int_array_array_new(p->partition.array[i][j].size);
        int_array_array rsigs[2]; TWICE(j) rsigs[j] = int_array_array_new(p->partition.array[i][j].size);
        TWICE(k) for(int j = 0; j < p->partition.array[i][k].size; ++j){
          int a = p->partition.array[i][k].array[j];
          // TODO : remove the sorting
          sigs[k].array[j] = signature(g[k], k, a);
          int_array_sort_less(&sigs[k].array[j]);
          rsigs[k].array[j] = signature(rg[k], k, a);
          int_array_sort_less(&rsigs[k].array[j]);
        }
        
        /* int_array hasha = int_array_new(sigsa.size); */
        /* for(int i = 0; i < sigsa.size; ++i){ */
        /*   hasha.array[i] = int_array_hash_bounded(&sigsa.array[i], &tmp_bounded) ^ int_array_hash_bounded(&sigsra.array[i], &tmp_bounded); */
        /* } */
        /* int_array hashb = int_array_new(sigsb.size); */
        /* for(int i = 0; i < sigsb.size; ++i){ */
        /*   hashb.array[i] = int_array_hash_bounded(&sigsb.array[i], &tmp_bounded) ^ int_array_hash_bounded(&sigsrb.array[i], &tmp_bounded); */
        /* } */

        /* // To partition the signatures, while preserving knowledge of which nodes these are the signatures */
        
        int_array I[2];
        TWICE(k) {
          I[k] = trivial_isomorphism(sigs[k].size);
          int I_cmp(int a, int b){
            return int_array_compare(&sigs[k].array[a], &sigs[k].array[b]);
          }
          int_array_sort(&I[k], I_cmp);
        }
        
        void local_free(){
          /* int_array_free(&hasha); */
          /* int_array_free(&hashb); */
          /* int_array_array_free(&sigsa); */
          /* int_array_array_free(&sigsb); */
          /* int_array_array_free(&sigsra); */
          /* int_array_array_free(&sigsrb); */
          /* int_array_free(&I); */
          /* int_array_free(&J); */
          /* int_array_free(&tmp_bounded); */
        }

        // Partition of signatures
        if(int_array_compare(&sigs[0].array[I[0].array[0]], &sigs[0].array[I[0].array[sigs[0].size-1]]) != 0 ||
           int_array_compare(&sigs[1].array[I[1].array[0]], &sigs[1].array[I[1].array[sigs[1].size-1]]) != 0 ||
           int_array_compare(&sigs[0].array[I[0].array[0]], &sigs[1].array[I[1].array[0]]) != 0){
          int j = 0;
          while(j != sigs[0].size){
            if(&sigs[0].array[I[0].array[j]] != &sigs[1].array[I[1].array[j]]){
              local_free();
              return false;
            }
            int k[2]; TWICE(l) k[l] = j + 1;
            TWICE(l) while(k[l] < sigs[l].size && int_array_compare(&sigs[l].array[I[l].array[j]], &sigs[l].array[I[l].array[k[l]]]) == 0){
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
          /* int_array_free(&pa->partition.array[i]); */
          /* int_array_free(&pb->partition.array[i]); */
          TWICE(j) p->partition.array[i][j] = int_array_empty();
        }
        /* int_array_free(&hasha); */
        /* int_array_free(&hashb); */
        /* int_array_array_free(&sigs[0]); */
        /* int_array_array_free(&sigs[1]); */
        /* int_array_array_free(&sigsra); */
        /* int_array_array_free(&sigsrb); */
        /* int_array_free(&I); */
        /* int_array_free(&J); */
      }
    }
  }
  wl_partition_cleanup(p);
  return true;
}

int_array graph_isomorphism_WL(graph (*g)[2]){
  TWICE(i) assert(g[i] != NULL);
  if(g[0]->size != g[1]->size){
    return int_array_empty();
  }

  graph (*rg)[2] = malloc(2 * sizeof(graph*));
  TWICE(i) *rg[i] = graph_reverse(g[i]);
  
  bool backtrack(wl_partition* p, int depth){
    printf("Backtrack depth %d\n", depth);
    if(!stable_partition(g, rg, p)){
      return false;
    }
    // TODO : better choice of i
    int i = 0;
    while(i < g[0]->size && p->partition.array[i][0].size == 1){
      i += 1;
    }
    // 1 element / class : isomorphism
    if(i == g[0]->size){
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
    /* partition_free(&pa); */
    /* partition_free(&pb); */
    /* graph_free(&ra); */
    /* graph_free(&rb); */
  }

  if(backtrack(&p, 0)){
    wl_partition_cleanup(&p);
    int_array iso = int_array_new(g[0]->size);
    for(int i = 0; i < g[0]->size; ++i){
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
  /* printf("Read graph 1\n"); */
  /* graph a = graph_read(); */
  /* printf("Read graph 2\n"); */
  /* graph b = graph_read(); */
  int sz; scanf("%d", &sz);
  graph a = graph_random(sz, sz);
  int_array in_iso = random_isomorphism(sz);
    for(int i = 0; i < a.size; ++i){
      printf("%d ", in_iso.array[i]);
    } printf("\n");
  graph b = graph_apply_isomorphism(&a, &in_iso);
  int_array_free(&in_iso);
  // Appel de l'algorithme
  int_array iso;
  graph (*g)[2] = malloc(2 * sizeof(graph*));
  if((iso = graph_isomorphism_WL(g)).size != 0){
    assert(test_isomorphism(&a, &b, &iso));
    printf("oui\n");
    for(int i = 0; i < a.size; ++i){
      printf("%d ", iso.array[i]);
    } printf("\n");
    int_array_free(&iso);
  }else{
    printf("non\n");
  }
  // Cleanup
  graph_free(&a);
  graph_free(&b);
  return 0;
}
