#! /bin/sh

slush_kb="kb/"
slush="/"
path_pre="path"

node_num=11
s_node_num=5
rtrs_num=3
#size=`expr 1024 \/ $node_num`
size=20

for i in `seq 1 11`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
done  
