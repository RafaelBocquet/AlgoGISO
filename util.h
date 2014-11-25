#ifndef ALGO_GISO_UTIL_H
#define ALGO_GISO_UTIL_H

#define SWAP(type, a, b)                          \
  {                                               \
    type tmp = (a);                               \
    a = b;                                        \
    b = tmp;                                      \
  }

const int ha = 1e9+7;
const int hc = 1e9+103;

int hash_combine(int seed, int value){
  return seed * ha ^ value;
}

#endif
