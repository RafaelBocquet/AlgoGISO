#ifndef ALGO_GISO_UTIL_H
#define ALGO_GISO_UTIL_H

#define SWAP(type, a, b)                          \
  {                                               \
    type tmp = (a);                               \
    a = b;                                        \
    b = tmp;                                      \
  }

int int_compare(int a, int b){
  return (a < b) ? -1 : (b < a);
}

unsigned hash_combine(unsigned seed, unsigned value){
  return seed ^ (value + 0x9e3779b9 + (seed<<6) + (seed>>2));
}

#endif
