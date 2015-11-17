#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include "list.h"
#include <pthread.h>

#define HASH_INDEX(_addr, _size_mask) (((_addr) >> 2) & (_size_mask))



template<class Ele, class Keytype>
class hash;

template<class Ele, class Keytype>
class hash {
private:
    unsigned my_size_log;
    unsigned my_size;
    unsigned my_size_mask;
    list<Ele, Keytype> *entries;

    list<Ele, Keytype> *get_list(unsigned the_idx);

public:
    pthread_cond_t *list_condition_vars;
    pthread_mutex_t *list_locks;

    void setup(unsigned the_size_log = 5);

    void insert(Ele *e);

    Ele *lookup(Keytype the_key);

    void print(FILE *f = stdout);

    void reset();

    void cleanup();

    void lock_list(Keytype);
    void unlock_list(Keytype key);

    void insert_with_lock(Ele *e) ;

    void merge(hash *other);

    Ele *lookup_with_lock(Keytype);

};

template<class Ele, class Keytype>
Ele *
hash<Ele, Keytype>::lookup_with_lock(Keytype the_key) {
    list<Ele, Keytype> *l;
    lock_list(the_key);

    l = &entries[HASH_INDEX(the_key, my_size_mask)];
    auto ret = l->lookup(the_key);
    unlock_list(the_key);
    return ret;
}

template<class Ele, class Keytype>
void hash<Ele, Keytype>::lock_list(Keytype key) {

    pthread_mutex_lock(&list_locks[HASH_INDEX(key, my_size_mask)]);

}


template<class Ele, class Keytype>
void hash<Ele, Keytype>::unlock_list(Keytype key) {

        pthread_mutex_unlock(&list_locks[HASH_INDEX(key, my_size_mask)]);

}

template<class Ele, class Keytype>
void hash<Ele, Keytype>::merge(hash *other) {
    if(!other) return;

    int i;
    list<Ele, Keytype> *l;
    Ele *e, *my_e;
    Keytype key;

    for(i = 0; i < other->my_size; ++i) {
	l = other->get_list(i);
        if(l != NULL) {
	    e = l->pop();
	    
	    while(e != NULL) {
		key = e->key();
		my_e = lookup(key);
		if(!my_e) {
		    insert(e);
		}
		else {
		    my_e->count += e->count;
		    delete e;
		}
		e = l->pop();
	    }
	}
    }
    
}


template<class Ele, class Keytype>
void
hash<Ele, Keytype>::setup(unsigned the_size_log) {
    my_size_log = the_size_log;
    my_size = 1 << my_size_log;
    my_size_mask = (1 << my_size_log) - 1;
    entries = new list<Ele, Keytype>[my_size];
    // we will have a lock for each of the lists in the hashtable
    list_locks = new pthread_mutex_t[my_size];
    // initialize all locks
    for (int i = 0; i < my_size; i++) {
        list_locks[i] = PTHREAD_MUTEX_INITIALIZER;
    }
}

template<class Ele, class Keytype>
list<Ele, Keytype> *
hash<Ele, Keytype>::get_list(unsigned the_idx) {
    if (the_idx >= my_size) {
        fprintf(stderr, "hash<Ele,Keytype>::list() public idx out of range!\n");
        exit(1);
    }
    return &entries[the_idx];
}

template<class Ele, class Keytype>
Ele *
hash<Ele, Keytype>::lookup(Keytype the_key) {
    list<Ele, Keytype> *l;

    l = &entries[HASH_INDEX(the_key, my_size_mask)];
    return l->lookup(the_key);
}


template<class Ele, class Keytype>
void
hash<Ele, Keytype>::print(FILE *f) {
    unsigned i;

    for (i = 0; i < my_size; i++) {
        entries[i].print(f);
    }
}

template<class Ele, class Keytype>
void
hash<Ele, Keytype>::reset() {
    unsigned i;
    for (i = 0; i < my_size; i++) {
        entries[i].cleanup();
    }
}

template<class Ele, class Keytype>
void
hash<Ele, Keytype>::cleanup() {
    unsigned i;
    reset();
    delete[] entries;
    delete[] list_locks;
}

template<class Ele, class Keytype>
void
hash<Ele, Keytype>::insert(Ele *e) {
    entries[HASH_INDEX(e->key(), my_size_mask)].push(e);
}

template<class Ele, class Keytype>
void
hash<Ele, Keytype>::insert_with_lock(Ele *e) {
    unsigned list_index = HASH_INDEX(e->key(), my_size_mask);

    pthread_mutex_lock(&list_locks[list_index]);

    entries[list_index].push(e);

    pthread_mutex_unlock(&list_locks[list_index]);
}


#endif
