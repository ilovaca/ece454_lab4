#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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
void worker(int num_samples);
// global mutex lock
pthread_mutex_t mutex;

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


int
main(int argc, char *argv[]) {
    int i, j, k;
    int rnum;
    unsigned key;
    sample *s;

    // deleted Print out team information
    //
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

    // initializing the mutex lock
    pthread_mutex_init(&mutex, NULL);

    // according to the number of threads, we start threads
    for (int i = 0; i < num_threads; ++i)
    {
    	pthread_create(&workers[i], NULL, worker, (void *) (NUM_SEED_STREAMS / num_threads));
    }
    // wait until they are all done with their work
    for (int i = 0; i < num_threads; ++i)
    {
    	pthread_join(worker[i]);
    }

    h.print();
}
    // process streams starting with different initial numbers
    for (i = 0; i < NUM_SEED_STREAMS; i++) {
        rnum = i;

        // collect a number of samples
        for (j = 0; j < SAMPLES_TO_COLLECT; j++) {

            // skip a number of samples
            for (k = 0; k < samples_to_skip; k++) {
                rnum = rand_r((unsigned int *) &rnum);
            }

            // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
            key = rnum % RAND_NUM_UPPER_BOUND;

            // if this sample has not been counted before
            if (!(s = h.lookup(key))) {

                // insert a new element for it into the hash table
                s = new sample(key);
                h.insert(s);
            }

            // increment the count for the sample
            s->count++;
        }
    }


    // print a list of the frequency of all samples
    h.print();
}


// we need to have functions that partitions the workload
/* this function runs the NUM_SEED_SAMPLE/num_threads parts
 The critical section is the insertion of the key, we need 
 a mutex here to lock the hash table!
*/
void worker(int num_samples){
	for (int i = 0; i < num_samples; i++) {
		for (int j = 0; j < )
	}
}