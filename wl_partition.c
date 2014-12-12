#include "wl_partition.h"

#include "stdio.h"

int wl_hash_f(int i){
  return i*i*i;
}

void wl_print_partition(wl_partition* p){
  for(int i = 0; i < p->partition.size; ++i) if(p->partition.array[i][0].size != 0) {
    printf("%d : \n", i);
    TWICE(k){
      printf("\t");
      for(int j = 0; j < p->partition.array[i][k].size; ++j){
        printf("%d(%d) ", p->partition.array[i][k].array[j], p->elements_hash[k].array[p->partition.array[i][k].array[j]]);
        fflush(stdout);
        assert(p->elements[k].array[p->partition.array[i][k].array[j]] == i);
      }
      printf("\n");
    }
  }
}

wl_partition wl_partition_empty(){
  wl_partition p;
  p.partition		      = int_array_pair_array_empty();
  TWICE(i) p.elements[i]      = int_array_empty();
  TWICE(i) p.elements_hash[i] = int_array_empty();
  p.update_queue	      = int_set_empty();
  return p;
}

bool wl_partition_is_empty(wl_partition* p){
  assert(p != NULL);
  return p->partition.size == 0 && p->elements[0].size == 0;
}

wl_partition wl_partition_new(int size){
  wl_partition p;
  p.partition		      = int_array_pair_array_empty();
  TWICE(i) {
    p.elements[i]      = int_array_new(size);
    p.elements_hash[i] = int_array_new(size);
    for(int j = 0; j < size; ++j){
      p.elements[i].array[j]      = -1;
      p.elements_hash[i].array[j] = 42;
    }
  }
  p.update_queue	      = int_set_empty();
  return p;
}

wl_partition wl_partition_new_with_classes(int size, int cls_size){
  wl_partition p = wl_partition_new(size);
  p.partition    = int_array_pair_array_new(cls_size);
  return p;
}

wl_partition wl_partition_copy(wl_partition* p){
  wl_partition q;
  q.partition = int_array_pair_array_copy(&p->partition);
  TWICE(i){
    q.elements[i] = int_array_copy(&p->elements[i]);
    q.elements_hash[i] = int_array_copy(&p->elements_hash[i]);
  }
  q.update_queue = int_set_copy(&p->update_queue);

  return q;
}

void wl_partition_free(wl_partition* p){
  assert(p != NULL);
  int_array_pair_array_free(&p->partition);
  TWICE(i){
    int_array_free(&p->elements[i]);
    int_array_free(&p->elements_hash[i]);
  }
  int_set_free(&p->update_queue);
}

int wl_partition_new_class(wl_partition* p){
  assert(p != NULL);
  int cls = p->partition.size;
  int_array a[2] = { int_array_empty(), int_array_empty() };
  int_array_pair_array_append(&p->partition, a);
  return cls;
}

void wl_partition_set_class(wl_partition* p, int cls, int a[2]){
  assert(p != NULL);
  TWICE(i) assert(a[i] >= 0 && a[i] < p->elements[i].size);
  assert(cls >= 0 && cls < p->partition.size);

  TWICE(i){
    // p->elements_hash[i].array[a[i]] += hash_f(cls) - hash_f(p->elements[i].array[a[i]]);
    p->elements[i].array[a[i]] = cls;
    int_array_append(&p->partition.array[cls][i], a[i]);
  }
}

void wl_partition_set_class_single(wl_partition* p, int cls, int i, int a){
  assert(p != NULL);
  assert(i == 0 || i == 1);
  assert(a >= 0 && a < p->elements[i].size);
  assert(cls >= 0 && cls < p->partition.size);

  // p->elements_hash[i].array[a] += hash_f(cls) - hash_f(p->elements[i].array[a]);
  p->elements[i].array[a] = cls;
  int_array_append(&p->partition.array[cls][i], a);
}

// false if partition is invalid (classes with different sizes)
bool wl_partition_cleanup(wl_partition* p){
  assert(p != NULL);
  
  int_array mapping = int_array_new(p->partition.size);
  int cur = 0;
  int_array_pair_array np = int_array_pair_array_empty();
  for(int i = 0; i < p->partition.size; ++i){
    if(p->partition.array[i][0].size != p->partition.array[i][1].size){
      // TODO : free here
      return false;
    }
    if(p->partition.array[i][0].size != 0){
      TWICE(k) for(int j = 0; j < p->partition.array[i][k].size; ++j){
	// p->elements_hash[k].array[p->partition.array[i][k].array[j]] += hash_f(cur) - hash_f(p->elements[k].array[p->partition.array[i][k].array[j]]);
        p->elements[k].array[p->partition.array[i][k].array[j]] = cur;
      }
      mapping.array[i] = cur;
      int_array_pair_array_append(&np, p->partition.array[i]);
      TWICE(j) p->partition.array[i][j] = int_array_empty();
      cur += 1;
    }
  }
  
  int mapping_f(int i){
    return mapping.array[i];
  }

  int_set_map_monotonous(&p->update_queue, mapping_f);
  
  int_array_free(&mapping);
  int_array_pair_array_free(&p->partition);
  p->partition = np;

  return true;
}

