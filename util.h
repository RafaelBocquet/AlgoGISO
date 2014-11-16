#ifndef ALGO_GISO_UTIL_H
#define ALGO_GISO_UTIL_H

#define SWAP(type, a, b)                          \
  {                                               \
    type tmp = (a);                               \
    a = b;                                        \
    b = tmp;                                      \
  }

size_t hash_combine(size_t seed, size_t value){
  return seed ^ (value + 0x9E3779B9 + (seed<<6) + (seed>>2));
}

#endif
