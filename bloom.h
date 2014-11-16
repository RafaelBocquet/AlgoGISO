#ifndef ALGO_GISO_BLOOM_H
#define ALGO_GISO_BLOOM_H

#include "stdlib.h"

typedef struct bloom_filter {
  int            size;
  unsigned char* buffer;
} bloom_filter;

bloom_filter bloom_filter_empty(int size){
  bloom_filter b;
  b.size = size;
  b.bufferSize = malloc(((size + 7) / 8) * sizeof(char));
  for(int i = 0; i < (size + 7) / 8; ++i){
    b.buffer = 0;
  }
  return b;
}

void bloom_filter_free(bloom_filter* b){
  assert(b != NULL);
  free(b->buffer);
}

#endif
