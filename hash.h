
#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include "list.h"

#define HASH_INDEX(_addr,_size_mask) (((_addr) >> 2) & (_size_mask))

// unsigned array_size = 0;
// pthread_mutex_t* list_locks = nullptr;

template<class Ele, class Keytype> class hash;

template<class Ele, class Keytype> class hash {
 private:
  unsigned my_size_log;
  unsigned my_size;
  unsigned my_size_mask;
  list<Ele,Keytype> *entries;
  list<Ele,Keytype> *get_list(unsigned the_idx);
  pthread_mutex_t* list_locks;
 public:
  void setup(unsigned the_size_log=5);
  void insert(Ele *e);
  Ele *lookup(Keytype the_key);
  void print(FILE *f=stdout);
  void reset();
  void cleanup();
  Ele *lookup_and_insert_if_absent(Keytype key);
};

/*This method looks up the key and will insert the key if absent.
 It provides mutually exclusive access to the lists
*/
template<class Ele, class Keytype> 
Ele * hash<Ele,Keytype>::lookup_and_insert_if_absent(Keytype key){
  unsigned list_index = HASH_INDEX(key,my_size_mask);
  // we only lock the list that we are accessing
  pthread_mutex_lock(&list_locks[list_index]);
  auto kp = entries[list_index].lookup(key);
  if (kp == nullptr) {
    // key does not exist
    kp = new Ele(key);
    entries[list_index].insert(kp);
    kp->count++;
  } else {
    kp->count++;
  }
  pthread_mutex_unlock(&list_locks[list_index]);
  return kp;
}

template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::setup(unsigned the_size_log){
  my_size_log = the_size_log;
  my_size = 1 << my_size_log;
  my_size_mask = (1 << my_size_log) - 1;
  entries = new list<Ele,Keytype>[my_size];
  // we will have a lock for each of the lists in the hashtable
  list_locks = new pthread_mutex_t[my_size];
  // initialize all locks
  for (int i = 0; i < my_size; i++) {
    list_locks[i] = PTHREAD_MUTEX_INITIALIZER;
  }
}

template<class Ele, class Keytype> 
list<Ele,Keytype> *
hash<Ele,Keytype>::get_list(unsigned the_idx){
  if (the_idx >= my_size){
    fprintf(stderr,"hash<Ele,Keytype>::list() public idx out of range!\n");
    exit (1);
  }
  return &entries[the_idx];
}

template<class Ele, class Keytype> 
Ele *
hash<Ele,Keytype>::lookup(Keytype the_key){
  list<Ele,Keytype> *l;

  l = &entries[HASH_INDEX(the_key,my_size_mask)];
  return l->lookup(the_key);
}  

template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::print(FILE *f){
  unsigned i;

  for (i=0;i<my_size;i++){
    entries[i].print(f);
  }
}

template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::reset(){
  unsigned i;
  for (i=0;i<my_size;i++){
    entries[i].cleanup();
  }
}

template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::cleanup(){
  unsigned i;
  reset();
  delete [] entries;
  delete [] list_locks;
}

template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::insert(Ele *e){
  entries[HASH_INDEX(e->key(),my_size_mask)].push(e);
}


#endif
