#! /bin/sh

slush_kb="kb/"
slush="/"
path_pre="path"

node_num=18
#node_num=3
rtrs_num=2
#size=`expr 1024 \/ $node_num`
size=70
sn=1
rep_which=0
load=10
: << '#_comment_out'
for i in `seq 1 10`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_fattree_web -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn -rep_which=$rep_which -load=$load"
    python ./pcap/file_delete2.py
done  
#_comment_out
load=20
for i in `seq 11 20`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_fattree_web -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn -rep_which=$rep_which -load=$load"
    python ./pcap/file_delete2.py
done  
load=30
for i in `seq 21 30`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_fattree_web -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn -rep_which=$rep_which -load=$load"
    python ./pcap/file_delete2.py
done  
load=40
for i in `seq 31 40`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_fattree_web -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn -rep_which=$rep_which -load=$load"
    python ./pcap/file_delete2.py
done  
load=50
for i in `seq 41 50`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_fattree_web -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn -rep_which=$rep_which -load=$load"
    python ./pcap/file_delete2.py
done  
load=60
for i in `seq 51 60`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_fattree_web -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn -rep_which=$rep_which -load=$load"
    python ./pcap/file_delete2.py
done  
load=70
for i in `seq 61 70`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_fattree_web -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn -rep_which=$rep_which -load=$load"
    python ./pcap/file_delete2.py
done  
load=80
for i in `seq 71 80`; do
	path=$size$slush_kb$path_pre$path_num$slush$i
	echo ${path}
	#sn=`expr $i - 1`
    ./waf --run "dce-iperf-mptcp_fattree_web -nDir=$i -s_size=$size -nNodes=$node_num -nRtrs=$rtrs_num -s_nodes=$sn -rep_which=$rep_which -load=$load"
    python ./pcap/file_delete2.py
done  
