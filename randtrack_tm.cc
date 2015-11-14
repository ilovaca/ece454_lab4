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
// to be called upon creating threads
void* worker_function(void* num_streams);

class sample;

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


int main(int argc, char *argv[]) {
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
    // initialize a 16K-entry (2**14) hash of empty lists
    h.setup(14);


    // according to the number of threads, we start threads
    for (unsigned long i = 0; i < num_threads; ++i)
    {
    	/*There are two ways to pass arguments to the worker function
			1. pass the argument as an "address", but at the worker function
			side we don't dereference it, instead we cast it into the desired 
			type --> pointer casting is extremely error prone!
			2. we dynamically allocate a variable and pass the pointer to the
			worker function, and delete it INSIDE the function
    	*/
    	// pthread_create(&workers[i], NULL, worker_function, (void*) ((unsigned long)(NUM_SEED_STREAMS / num_threads)));
    	// unsigned long* arg = new unsigned long;
    	// *arg = (NUM_SEED_STREAMS / num_threads);
    	// std::cout<<"argument value: "<<*arg<<std::endl;
    	pthread_create(&workers[i], nullptr, worker_function, (unsigned long*)i);
    	// this is probably a data race here, when I passed the argument 
    	// to the thread worker, the arg may have already been deleted 
    	// in the line below
    	/*delete arg;*/
    }
    // wait until they are all done with their work
    for (int i = 0; i < num_threads; ++i)
    {
    	pthread_join(workers[i], NULL);
    }

    h.print();
}


// we need to have functions that partitions the workload
/* this function runs the NUM_SEED_SAMPLE/num_threads parts
 The critical section is the insertion of the key, we need 
 a mutex here to lock the hash table!
*/
void* worker_function(void* ith_thread){
	sample* s = nullptr;
	unsigned key;
	// the line below is where we interpret the address just as a regular variable, No dereferencing
	// unsigned long numStreams = (unsigned long) num_streams;

	// num_streams = (unsigned*) num_streams;
	// auto temp = static_cast<unsigned long*>(num_streams);
	// auto numStreams = *(temp);
	auto ith_slice = (unsigned long) ith_thread;
    auto slice_size = NUM_SEED_STREAMS / num_threads;

    for (int i = ith_slice * slice_size; i < slice_size*(ith_slice + 1); i++) {
        int rnum = i;
        // For each stream, we collect a number of samples
        for (int j = 0; j < SAMPLES_TO_COLLECT; j++) {

            // skip a number of samples
            for (int k = 0; k < samples_to_skip; k++) {
                rnum = rand_r((unsigned int *) &rnum);
            }

            // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
            key = rnum % RAND_NUM_UPPER_BOUND;

            __transaction_atomic{
            if (!(s = h.lookup(key))) {

                // insert a new element for it into the hash table
                s = new sample(key);
                h.insert(s);
            }
            // increment the count for the sample
            s->count++;
        }
        }   
    }

	// delete(temp);
	return nullptr;
} 