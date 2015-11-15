#!/bin/bash
make clean
make randtrack
make randtrack_global_lock
make randtrack_list_lock
make randtrack_tm
make randtrack_reduction
# make randtrack_element_lock
NUM_THREADS=4
SAMPLES_TO_SKIP=50

# base 
randtrack 1 $SAMPLES_TO_SKIP > rt1.out
sort -n rt1.out > rt1.outs

if [ "$1" = "global" ] 
then
	echo -e "Global Lock version, NUM_THREADS == $NUM_THREADS SAMPLES_TO_SKIP == $SAMPLES_TO_SKIP"
	randtrack_global_lock $NUM_THREADS $SAMPLES_TO_SKIP > rt_global.out
	sort -n rt_global.out > rt_global.outs
	diff rt1.outs rt_global.outs > diff1.out
elif [ "$1" = "tm" ] 
then
	echo -e "TM version, NUM_THREADS == $NUM_THREADS SAMPLES_TO_SKIP == $SAMPLES_TO_SKIP"
	randtrack_tm $NUM_THREADS $SAMPLES_TO_SKIP > rt_tm.out
	sort -n rt_tm.out > rt_tm.outs
	diff rt1.outs rt_tm.outs > diff2.out
elif [ "$1" = "list" ] 
then
	echo -e "List Lock version, NUM_THREADS == $NUM_THREADS SAMPLES_TO_SKIP == $SAMPLES_TO_SKIP"
	randtrack_list_lock $NUM_THREADS $SAMPLES_TO_SKIP > rt_list.out
	sort -n rt_list.out > rt_list.outs
	diff rt1.outs rt_list.outs > diff3.out
elif [ "$1" = "reduction" ] 
then
	echo -e "reduction version, NUM_THREADS == $NUM_THREADS SAMPLES_TO_SKIP == $SAMPLES_TO_SKIP"
	randtrack_reduction $NUM_THREADS $SAMPLES_TO_SKIP > rt_reduction.out
	sort -n rt_reduction.out > rt_reduction.outs
	diff rt1.outs rt_reduction.outs > diff4.out
elif [ "$1" = "element" ]
then
	echo -e "Element lock version, NUM_THREADS == $NUM_THREADS SAMPLES_TO_SKIP == $SAMPLES_TO_SKIP"
	randtrack_element_lock $NUM_THREADS $SAMPLES_TO_SKIP > rt_element_lock.out
	sort -n rt_element_lock.out > rt_element_lock.outs
	diff rt1.outs rt_element_lock.outs > diff5.out
fi
