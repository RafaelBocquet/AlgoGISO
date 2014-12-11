#include "set.h"
#include "assert.h"
#include "stdio.h"

set_node* make_set_node(int value, set_node* l, set_node* r){
  set_node* s = malloc(sizeof(set_node));
  s->value = value;
  s->l = l;
  s->r = r;
  return s;
}

int_set int_set_empty(){
  return NULL;
}

void int_set_free(int_set* s){
  assert(s != NULL);
  if(*s != NULL){
    int_set_free(&(*s)->l);
    int_set_free(&(*s)->r);
    free(*s);
  }
}

int_set int_set_copy(int_set* s){
  assert(s != NULL);
  if((*s) == NULL){
    return NULL;
  }else{
    return make_set_node((*s)->value, int_set_copy(&(*s)->l), int_set_copy(&(*s)->r));
  }
}

bool int_set_is_empty(int_set* s){
  assert(s != NULL);
  return (*s) == NULL;
}

void int_set_print(int_set* s){
  assert(s != NULL);
  if(*s != NULL){
    int_set_print(&(*s)->l);
    printf("%d ", (*s)->value);
    int_set_print(&(*s)->r);
  }
}

void splay_zig(set_node** s, bool d){
  if(d == false){
    set_node ns = { (*s)->l->value, (*s)->l->l, (*s)->l };
    set_node nsl = { (*s)->value, (*s)->l->r, (*s)->r };
    *(*s)->l = nsl;
    *(*s) = ns;
  }else{
    set_node ns = { (*s)->r->value, (*s)->r, (*s)->r->r };
    set_node nsr = { (*s)->value,  (*s)->l, (*s)->r->l };
    *(*s)->r = nsr;
    *(*s) = ns;
  }
}

void splay_zigzig(set_node** p, bool d){
  if(d == false){
    set_node np = { (*p)->l->l->value, (*p)->l->l->l, (*p)->l };
    set_node ns = { (*p)->l->value, (*p)->l->l->r, (*p)->l->l };
    set_node nsl = { (*p)->value, (*p)->l->r, (*p)->r };
    *(*p)->l->l = nsl;
    *(*p)->l = ns;
    *(*p) = np;
  }else{
    set_node np = { (*p)->r->r->value, (*p)->r, (*p)->r->r->r };
    set_node ns = { (*p)->r->value, (*p)->r->r, (*p)->r->r->l };
    set_node nsr = { (*p)->value, (*p)->l, (*p)->r->l };
    *(*p)->r->r = nsr;
    *(*p)->r = ns;
    *(*p) = np;
  }
}

void splay_zigzag(set_node** p, bool d){
  if(d == false){
    set_node np = { (*p)->l->r->value, (*p)->l, (*p)->l->r };
    set_node ns = { (*p)->l->value, (*p)->l->l, (*p)->l->l->l };
    set_node nsr = { (*p)->value, (*p)->l->l->r, (*p)->r };
    *(*p)->l->r = nsr;
    *(*p)->l = ns;
    *(*p) = np;
  }else{
    set_node np = { (*p)->r->l->value, (*p)->r->l, (*p)->r };
    set_node ns = { (*p)->r->value, (*p)->r->r->r, (*p)->r->r };
    set_node nsr = { (*p)->value, (*p)->l, (*p)->r->r->l };
    *(*p)->r->l = nsr;
    *(*p)->r = ns;
    *(*p) = np;
  }
}


// True if the element was inserted
bool int_set_insert(int_set* s, int value){
  assert(s != NULL);
  bool rt;
  bool insert_(set_node** p, set_node** s, bool d, int value){
    if(*s == NULL){
      rt = true;
      *s = make_set_node(value, NULL, NULL);
      return true;
    }else if(value == (*s)->value){
      rt = false;
      return true;
    }else{
      bool d_ = (value >= (*s)->value);
      if(insert_(s, d_?&(*s)->r:&(*s)->l, d_, value)){
        if(p == NULL){
          splay_zig(s, d_);
        }else if(d == d_){
          splay_zigzig(p, d);
        }else{
          splay_zigzag(p, d);
        }
        return false;
      }else{
        return true;
      }
    }
  }
  insert_(NULL, s, false, value);
  return rt;
}

int int_set_delete(int_set* s){
  assert(s != NULL);
  assert(*s != NULL); // Not empty
  int rt;
  bool delete_(set_node** p, set_node** s, bool d){
    if((*s)->r == NULL){
      rt = (*s)->value;
      set_node* s_ = *s;
      *s = (*s)->l;
      free(s_);
      return false;
    }else if((*s)->l == NULL){
      rt = (*s)->value;
      set_node* s_ = *s;
      *s = (*s)->r;
      free(s_);
      return false;
    }else{
      bool d_ = !d;
      if(delete_(s, d_?&(*s)->r:&(*s)->l, d_)){
        if(p == NULL){
          splay_zig(s, d_);
        }else if(d == d_){
          splay_zigzig(p, d);
        }else{
          splay_zigzag(p, d);
        }
        return false;
      }else{
        return true;
      }
    }
  }
  delete_(NULL, s, false);
  return rt;
}

void int_set_map_monotonous(int_set* s, int(*f)(int)){
  assert(s != NULL);
  if(*s != NULL){
    int_set_map_monotonous(&(*s)->l, f);
    (*s)->value = f((*s)->value);
    int_set_map_monotonous(&(*s)->r, f);
  }
}
