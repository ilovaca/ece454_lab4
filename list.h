#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <vector>
template<class Ele, class Keytype>
class list;
extern     pthread_cond_t *list_condition_vars;
extern     pthread_mutex_t *list_locks;

template<class Ele, class Keytype>
class list {
private:
    Ele *my_head;
    std::vector<pthread_mutex_t> element_mutex_v;
    pthread_mutex_t head_lock;
    unsigned long long my_num_ele;
public:
    list() {
        my_head = NULL;
        my_num_ele = 0;
    }

    void setup();

    unsigned num_ele() { return my_num_ele; }

    Ele *head() { return my_head; }

    Ele *lookup(Keytype the_key);

    Ele *lookup_with_lock(Keytype the_key, unsigned list_index);

    Ele *lookup_with_lock(Keytype the_key);


    void push(Ele *e);

    void push_with_lock(Ele *e);

    Ele *pop();

    void print(FILE *f = stdout);

    void cleanup();

    void merge(list& other);

    // Ele * lookup_and_insert_with_lock(Keytype the_key, unsigned index_in_entries);
};

template<class Ele, class Keytype>
Ele *
list<Ele, Keytype>::lookup_with_lock(Keytype the_key) {  
    // before looking up the list, we need to check if there's anyone inserting elements
    // pthread_cond_wait(&list_condition_vars[list_index], &list_locks[list_index]);

    if (my_head == nullptr) return nullptr;
    auto temp = my_head;
    do {
        if (temp->key() == the_key) {
            return temp;
        }
        auto prev = temp;
        temp = temp->next;

    } while (temp != nullptr);
    return nullptr; 

}


template<class Ele, class Keytype>
Ele *
list<Ele, Keytype>::lookup_with_lock(Keytype the_key, unsigned list_index) {  
    // before looking up the list, we need to check if there's anyone inserting elements
    // pthread_cond_wait(&list_condition_vars[list_index], &list_locks[list_index]);

    if (my_head == nullptr) return nullptr;
    auto temp = my_head;
    do {
        pthread_mutex_lock(&(temp->lock));
        if (temp->key() == the_key) {
            pthread_mutex_unlock(&(temp->lock));
            return temp;
        }
        auto prev = temp;
        temp = temp->next;
        pthread_mutex_unlock(&(prev->lock));

    } while (temp != nullptr);
    return nullptr; 

}


// template<class Ele, class Keytype>
// Ele *
// list<Ele, Keytype>::lookup_and_insert_with_lock(Keytype the_key, unsigned index_in_entries) {  
//     bool found = false;
//     if (my_head == nullptr) {
//         pthread_mutex_lock(&(h->list_locks[index_in_entries]));
//         auto kp = new Ele(key);
//         this.push(kp);
//         kp->count++;
//         pthread_mutex_unlock(&(h->list_locks[index_in_entries]));
//         return kp;
//     }

//     auto temp = my_head;
//     do {
//         pthread_mutex_lock(&(temp->lock));
//         if (temp->key() == the_key) {
//             found = true;
//             temp->count++;
//             pthread_mutex_unlock(&(temp->lock));
//             return temp;
//         }
//         auto prev = temp;
//         temp = temp->next;
//         pthread_mutex_unlock(&(prev->lock));

//     } while (temp != nullptr);
//     if (!found) {
//         thread_mutex_lock(&(h->list_locks[index_in_entries]));
//         auto kp = new Ele(key);
//         this.push(kp);
//         kp->count++;
//         pthread_mutex_unlock(&(h->list_locks[index_in_entries]));
//         return kp;
//     }
//     return nullptr; 

// }


template<class Ele, class Keytype>
void list<Ele, Keytype>::merge(list& other) {
    for (auto p = other.my_head; p != nullptr; p = p -> next) {
        auto key = p->key();
        if (this->lookup(key) != nullptr) {
            // the key in the other list exists in this list, we increment the count
            std::cout<<this->lookup(key)<<std::endl;
            this->lookup(key)->count += p->count; 
        } else {
            // the key does not exist in this list, so we create a new entry and insert it
            auto new_ele = new Ele(key);
            new_ele->count = p->count;
            this->push(new_ele);
        }
    }
}


template<class Ele, class Keytype>
void
list<Ele, Keytype>::setup() {
    my_head = NULL;
    my_num_ele = 0;
    head_lock = PTHREAD_MUTEX_INITIALIZER;
}

template<class Ele, class Keytype>
void
list<Ele, Keytype>::push(Ele *e) {
    e->next = my_head;
    my_head = e;
    my_num_ele++;
}

template<class Ele, class Keytype>
void
list<Ele, Keytype>::push_with_lock(Ele *e) {
    pthread_mutex_lock(&head_lock);
    e->next = my_head;
    my_head = e;
    pthread_mutex_unlock(&head_lock);
    pthread_mutex_t mutex;
    element_mutex_v.push_back(mutex);
    element_mutex_v.back() = PTHREAD_MUTEX_INITIALIZER;
    my_num_ele++;
}


template<class Ele, class Keytype>
Ele *
list<Ele, Keytype>::pop() {
    // we need to lock the head element before we pop it
    // pthread_mutex_lock(&element_mutex_v[my_num_ele - 1]);
    Ele *e;
    e = my_head;
    if (e) {
        my_head = e->next;
        my_num_ele--;
    }
    // pthread_mutex_unlock(&element_mutex_v[my_num_ele - 1]);
    // element_mutex_v.pop_back();
    return e;
}

template<class Ele, class Keytype>
void
list<Ele, Keytype>::print(FILE *f) {
    Ele *e_tmp = my_head;

    while (e_tmp) {
        e_tmp->print(f);
        e_tmp = e_tmp->next;
    }
}

template<class Ele, class Keytype>
Ele *
list<Ele, Keytype>::lookup(Keytype the_key) {
    Ele *e_tmp = my_head;

    while (e_tmp && (e_tmp->key() != the_key)) {
        e_tmp = e_tmp->next;
    }
    return e_tmp;
}



template<class Ele, class Keytype>
void
list<Ele, Keytype>::cleanup() {
    Ele *e_tmp = my_head;
    Ele *e_next;

    while (e_tmp) {
        e_next = e_tmp->next;
        delete e_tmp;
        e_tmp = e_next;
    }
    my_head = NULL;
    my_num_ele = 0;
}

#endif
