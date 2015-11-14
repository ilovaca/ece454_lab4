#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include "defs.h"
#include "hash.h"

#define SAMPLES_TO_COLLECT   10000000
#define RAND_NUM_UPPER_BOUND   100000
#define NUM_SEED_STREAMS            4

/* 
 * ECE454 Students: 
 * Please fill in the following team struct 
 */
team_t team = {
        "Stupid Team",                  /* Team name */

        "Yeqi Shi",                    /* First member full name */
        "1000274277",                 /* First member student number */
        "yeqi.shi@mail.utoronto.ca",                 /* First member email address */

        "Yi Fan Shao",                           /* Second member full name */
        "",                           /* Second member student number */
        "samyifan.shao@mail.utoronto.ca"                            /* Second member email address */
};

unsigned num_threads;
unsigned samples_to_skip;
// the worker function that completes a portion of the samples
void* worker_function(void* num_streams);
void hash_reduction();
class sample;
void merge_hash(hash<sample, unsigned>* h1, hash<sample,unsigned>* h2);

class sample {
    unsigned my_key;
public:
    sample *next;
    unsigned count;

    sample(unsigned the_key) {
        my_key = the_key;
        count = 0;
    };

    unsigned key() { return my_key; }

    void print(FILE *f) { printf("%d %d\n", my_key, count); }
};

// This instantiates an empty hash table
// it is a C++ template, which means we define the types for
// the element and key value here: element is "class sample" and
// key value is "unsigned".  
hash<sample, unsigned> h;
hash<sample, unsigned>* private_hashes = nullptr;

int
main(int argc, char *argv[]) {
    int i, j, k;
    int rnum;
    unsigned key;
    sample *s;

    // deleted Print out team information
    // Print out team information
    // printf("Team Name: %s\n", team.team);
    // printf("\n");
    // printf("Student 1 Name: %s\n", team.name1);
    // printf("Student 1 Student Number: %s\n", team.number1);
    // printf("Student 1 Email: %s\n", team.email1);
    // printf("\n");
    // printf("Student 2 Name: %s\n", team.name2);
    // printf("Student 2 Student Number: %s\n", team.number2);
    // printf("Student 2 Email: %s\n", team.email2);
    // printf("\n");
    // Parse program argumentadditionals
    if (argc != 3) {
        printf("Usage: %s <num_threads> <samples_to_skip>\n", argv[0]);
        exit(1);
    }
    sscanf(argv[1], " %d", &num_threads);
    sscanf(argv[2], " %d", &samples_to_skip);

    // define Pthreads
    pthread_t workers[num_threads];
    private_hashes = new hash<sample, unsigned>[num_threads];
    // initialize a 16K-entry (2**14) hash of empty lists
    h.setup(14);

    // create and start threads
    for (unsigned long i = 0; i < num_threads; ++i)
    {
        // setup each hash table for every thread
        private_hashes[i].setup(14);
    	pthread_create(&workers[i], nullptr, worker_function, (unsigned long*) i);
    }
    // wait until they are all done with their work
    for (int i = 0; i < num_threads; ++i)
    {
    	pthread_join(workers[i], NULL);
    }
    // reduction part
    if (num_threads == 1) {
        h = hash_array[0];
    }
    else{
        auto merged_hash = hash_reduction(private_hashes,0,num_threads - 1);
        h = *merged_hash;
    }
    delete [] private_hashes;
    h.print();
}


hash<sample,unsigned>* hash_reduction(hash<sample, unsigned>* hash_array, unsigned left, unsigned right) {
    // if left == right then we've done merging the 
    if (right - left <= 0) return hash_array;
    unsigned mid = (left + right) / 2;
    
    merge_hash(hash_reduction(hash_array, left, mid),hash_reduction(hash_array, mid + 1, right));
}

void merge_hash(hash<sample, unsigned>* h1, hash<sample,unsigned>* h2){
    if(h1 == nullptr || h2 == nullptr) return;
    h1->merge(*(h2));
}

void* worker_function(void* ith_thread){
	sample* s = nullptr;
	unsigned key;

    auto ith_slice = (unsigned long) ith_thread;
	auto slice_size = NUM_SEED_STREAMS / num_threads;

	for (int i = ith_slice * slice_size; i < slice_size*(ith_slice+1); i++) {
		// std::cout<<"at "<<i<<"th stream"<<std::endl;
		int rnum = i;
        // For each stream, we collect a number of samples
        for (int j = 0; j < SAMPLES_TO_COLLECT; j++) {

            // skip a number of samples
            for (int k = 0; k < samples_to_skip; k++) {
                rnum = rand_r((unsigned int *) &rnum);
            }

            // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
            key = rnum % RAND_NUM_UPPER_BOUND;


            if (!(s = h.lookup(key))) {

                // insert a new element for it into the hash table
                s = new sample(key);
                h.insert(s);
            }
            s->count++;
		}	
	}
	// delete(temp);
	return nullptr;
} 