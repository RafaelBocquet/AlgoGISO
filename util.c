#include "util.h"

int int_rotate(int a){
  return (a >> 16) + (a << 16);
}

int int_compare(int a, int b){
  return (a < b) ? -1 : (b < a);
}

