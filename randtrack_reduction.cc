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
        "AYYLMAO",                  /* Team name */

        "Yeqi Shi",                    /* First member full name */
        "1000274277",                 /* First member student number */
        "yeqi.shi@mail.utoronto.ca",                 /* First member email address */

        "Yi Fan Shao",                           /* Second member full name */
        "",                           /* Second member student number */
        "samyifan.shao@mail.utoronto.ca"                            /* Second member email address */
};

unsigned num_threads;
unsigned samples_to_skip;

void *worker_function(void *num_streams);
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
hash<sample, unsigned> *h_table;

int
main(int argc, char *argv[]) {
    int i, j, k;
    int rnum;
    unsigned key;
    sample *s;

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

    // Parse program arguments
    if (argc != 3) {
        printf("Usage: %s <num_threads> <samples_to_skip>\n", argv[0]);
        exit(1);
    }
    sscanf(argv[1], " %d", &num_threads); // not used in this single-threaded version
    sscanf(argv[2], " %d", &samples_to_skip);

    pthread_t workers[num_threads];
    h_table = new hash<sample,unsigned> [num_threads];
    
    // initialize a 16K-entry (2**14) hash of empty lists
    h.setup(14);

    for (unsigned long i = 0; i < num_threads; ++i) {
	h_table[i].setup(14);
	pthread_create(&workers[i], NULL, worker_function, (unsigned long*) i);
    }

    for (i = 0; i < num_threads; ++i) {
	pthread_join(workers[i], NULL);
    }

    for (i = 0; i < num_threads; ++i) {
	h.merge(&h_table[i]);
    }

    // print a list of the frequency of all samples
    h.print();
}
void* worker_function(void* ith_thread){
    sample* s = nullptr;
    unsigned key;
    
    int ith_slice = (unsigned long) ith_thread;
    int slice_size = NUM_SEED_STREAMS / num_threads;
    
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
                h_table[ith_slice].insert(s);
            }
            s->count++;
	}	
    }
    // delete(temp);
    return nullptr;
}
