
CC = g++-4.9
CFLAGS = -O3 -std=c++11
DEBUG = -g -std=c++11
PTHREAD_ENABLE = -lpthread -pthread
TM_ENABLE = -fgnu-tm

all: randtrack 
		

randtrack: list.h hash.h defs.h randtrack.cc
	$(CC) $(CFLAGS) $(PTHREAD_ENABLE) randtrack.cc -o randtrack
randtrack_global_lock: list.h hash.h defs.h randtrack_global_lock.cc
	$(CC) $(CFLAGS) $(PTHREAD_ENABLE) randtrack_global_lock.cc -o randtrack_global_lock
randtrack_tm: list.h hash.h defs.h randtrack_tm.cc
	$(CC) $(CFLAGS) $(TM_ENABLE) $(PTHREAD_ENABLE) randtrack_tm.cc -o randtrack_tm
randtrack_list_lock: list.h hash.h defs.h randtrack_list_lock.cc
	$(CC) $(CFLAGS) $(PTHREAD_ENABLE) randtrack_list_lock.cc -o randtrack_list_lock
randtrack_reduction:  list.h hash.h defs.h randtrack_reduction.cc
	$(CC) $(CFLAGS) $(PTHREAD_ENABLE) randtrack_reduction.cc -o randtrack_reduction
randtrack_element_lock:  list.h hash_element_lock.h defs.h randtrack_element_lock.cc
	$(CC) $(CFLAGS) $(PTHREAD_ENABLE) randtrack_element_lock.cc -o randtrack_element_lock

randtrack_element_lock_debug:  list.h hash.h defs.h randtrack_element_lock.cc
	$(CC) $(DEBUG) $(PTHREAD_ENABLE) randtrack_element_lock.cc -o randtrack_element_lock
clean:
	rm -f *.o randtrack randtrack_global_lock randtrack_tm randtrack_list_lock
