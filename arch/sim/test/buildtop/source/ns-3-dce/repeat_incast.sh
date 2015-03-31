#! /bin/sh

slush_kb="kb/"
slush="/"
path_pre="path"

node_num=18
rtrs_num=2
#size=`expr 1024 \/ $node_num`
size=70
 : << '#_comment_out'
sn=1
for i in `seq 1 1`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_incast -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn"
    python ./pcap/file_delete2.py
done  

sn=2
for i in `seq 11 13`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_incast -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn"
    python ./pcap/file_delete2.py
done  
sn=3
for i in `seq 21 21`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_incast -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn"
    python ./pcap/file_delete2.py
done  
sn=4
for i in `seq 31 31`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_incast -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn"
    python ./pcap/file_delete2.py
done  
sn=5
for i in `seq 41 41`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_incast -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn"
    python ./pcap/file_delete2.py
done  
sn=6
for i in `seq 51 51`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_incast -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn"
    python ./pcap/file_delete2.py
done  
sn=7
for i in `seq 61 61`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_incast -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn"
    python ./pcap/file_delete2.py
done  
sn=8
for i in `seq 71 71`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_incast -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn"
    python ./pcap/file_delete2.py
done  
sn=9
for i in `seq 81 81`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_incast -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn"
    python ./pcap/file_delete2.py
done  
#_comment_out
sn=10
for i in `seq 91 91`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_incast -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn"
    python ./pcap/file_delete2.py
done  
