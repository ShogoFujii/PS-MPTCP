#! /bin/sh

slush_kb="kb/"
slush="/"
path_pre="path"

node_num=11
rtrs_num=2
#size=`expr 1024 \/ $node_num`
size=70

for i in `seq 1 11`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i \* 4`
	sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_param_sl -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn"
    python ./pcap/file_delete2.py
done  
