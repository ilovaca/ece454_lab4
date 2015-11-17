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

    Ele *lookup_and_insert_if_absent(Keytype key);
    
    Ele *lookup_with_lock(Keytype the_key);

    void insert_with_lock(Ele *e) ;

    void merge(hash *other);
    void lookup_and_insert_element_lock(Keytype key);

};


template<class Ele, class Keytype>
void hash<Ele, Keytype>::lookup_and_insert_element_lock(Keytype key) {
    unsigned list_index = HASH_INDEX(key, my_size_mask);
    auto kp = entries[list_index].lookup(key);
    // entires[list_index].lookup_with_lock(key);
    if (kp == nullptr) {
        // key does not exist, we need to lock the entire list and insert it
        pthread_mutex_lock(&list_locks[list_index]);
        kp = new Ele(key);
        entries[list_index].push(kp);
        kp->incre_count_with_lock();
        // we've done the insertion, we sigal the lookup threads to run
        // pthread_cond_signal(&list_condition_vars[list_index]);
        pthread_mutex_unlock(&list_locks[list_index]);
    } else {
        // key exists we just increment the counter
        kp->incre_count_with_lock();
    }
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

/*This method looks up the key and will insert the key if absent.
 It provides mutually exclusive access to the lists
*/
template<class Ele, class Keytype>
Ele *hash<Ele, Keytype>::lookup_and_insert_if_absent(Keytype key) {
    unsigned list_index = HASH_INDEX(key, my_size_mask);
    // we only lock the list that we are accessing
    pthread_mutex_lock(&list_locks[list_index]);
    auto kp = entries[list_index].lookup(key);
    if (kp == nullptr) {
        // key does not exist
        kp = new Ele(key);
        entries[list_index].push(kp);
        kp->count++;
    } else {
        kp->count++;
    }
    pthread_mutex_unlock(&list_locks[list_index]);
    return kp;
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
    list_condition_vars = new pthread_cond_t[my_size];
    // initialize all locks
    for (int i = 0; i < my_size; i++) {
        list_locks[i] = PTHREAD_MUTEX_INITIALIZER;
        list_condition_vars[i] = PTHREAD_COND_INITIALIZER;
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
Ele *
hash<Ele, Keytype>::lookup_with_lock(Keytype the_key) {
    list<Ele, Keytype> *l;

    l = &entries[HASH_INDEX(the_key, my_size_mask)];
    return l->lookup_with_lock(the_key);
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
