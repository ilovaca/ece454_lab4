#!/bin/bash
make clean
make randtrack
make randtrack_global_lock
make randtrack_list_lock
make randtrack_tm


# base 
# randtrack 1 1000 > rt1.out
# sort -n rt1.out > rt1.outs

if [ "$1" = "global" ] 
then
	echo -e "# Base version VS. global lock"
	randtrack_global_lock 4 1000 > rt_global.out
	sort -n rt_global.out > rt_global.outs
	diff rt1.outs rt_global.outs > diff1.out
elif [ "$1" = "tm" ] 
then
	echo -e "TM version"
	randtrack_tm 4 1000 > rt_tm.out
	sort -n rt_tm.out > rt_tm.outs
	diff rt1.outs rt_tm.outs > diff2.out
elif [ "$1" = "list" ] 
then
	echo -e "List Lock version"
	randtrack_list_lock 4 1000 > rt_list.out
	sort -n rt_list.out > rt_list.outs
	diff rt1.outs rt_list.outs > diff3.out
fi
