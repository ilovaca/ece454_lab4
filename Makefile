
CC = g++-4.9
CFLAGS = -O3 -std=c++11
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

clean:
	rm -f *.o randtrack randtrack_global_lock randtrack_tm randtrack_list_lock
