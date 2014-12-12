#ifndef ALGO_GISO_UTIL_H
#define ALGO_GISO_UTIL_H

#define SWAP(type, a, b)                          \
  {                                               \
    type tmp = (a);                               \
    a = b;                                        \
    b = tmp;                                      \
  }

#define TWICE(x) for(int x = 0; x <= 1; x += 1)

int int_rotate(int a);

int int_compare(int a, int b);

#endif
