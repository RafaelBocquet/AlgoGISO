#include "util.h"

int int_compare(int a, int b){
  return (a < b) ? -1 : (b < a);
}

unsigned hash_combine(unsigned seed, unsigned value){
  return seed ^ (value + 0x9e3779b9 + (seed<<6) + (seed>>2));
}
