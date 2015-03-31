#include <iostream>
#include <cstdlib>
#include <map>
#include <string.h>
#include <math.h>
#include <sstream>
#include <vector>
#include <algorithm>

#include "ns3/network-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/dce-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/constant-position-mobility-model.h"

using namespace ns3;
using namespace std;

//--script--//

void init_rand(){
	srand((unsigned) time(NULL));
}

bool random_slot(double ratio){
	//cout << rand() % 100 << endl;
	if (rand() % 100 < ratio * 100){
		return true;
	}
	return false;
}

void show_multimap(multimap<int, string> &adrs_set){
	multimap<int, string>::iterator it = adrs_set.begin();
	while(it != adrs_set.end()){
		cout << "From host" << (*it).first << ": [Dest_IP]" << (*it).second << endl;
		++it;
	}
	cout << "Total patterns : " << (unsigned int)adrs_set.size() << endl;
}
void show_map(map<int, string> &adrs_set){
	map<int, string>::iterator it = adrs_set.begin();
	while(it != adrs_set.end()){
		cout << "From host" << (*it).first << ": [Dest_IP]" << (*it).second << endl;
		++it;
	}
	cout << "Total patterns : " << (unsigned int)adrs_set.size() << endl;
}

int host_detect(char *adrs){
	const char delm[] = ".";
	char *tok;
	int host;

	tok = strtok(adrs, delm);
	int j = 0;
	while(tok != NULL){
		if(j == 1){
			host = atoi(tok)-1;
			break;
		}
		tok = strtok(NULL, delm);
		j++;
	}
	return host;
}

string double2string(double d){
	string rt;
	stringstream ss;
	ss << d;
	ss >> rt;
	return rt;
}

string int2string(int d){
	string rt;
	stringstream ss;
	ss << d;
	ss >> rt;
	return rt;
}

char* string2char(string s){
	int len = s.length();
	char* c = new char[len+1];
	memcpy(c, s.c_str(), len+1);
	return c;
}
string fix_adrs(char *adrs, int path){
	const char delm[] = ".";
	char *tok;
	int post;
	int host;
	string fix="";

	tok = strtok(adrs, delm);
	int j = 0;
	while(tok != NULL){
		if(j == 1){
			host = atoi(tok);
		}else if(j == 2){
			post = atoi(tok);
		}
		tok = strtok(NULL, delm);
		j++;
	}
	if(path == 2){
		if(host == 2){
			host = 1;
		}else if (host == 4){
			host = 3;
		}
	}else if (path == 3){
		if(host == 4){
			host = 3;
		}
	}
	fix=fix+"10."+int2string(host)+"."+int2string(post)+".2";
	return fix;
}

void socketTraffic(multimap<int, string> &ip_set, uint32_t byte, double start, double end, NodeContainer nodes){
	uint32_t size = byte;
		if (byte != 0){
			//size = byte * 1000 - 1502;
			size = byte * 1024;
		}
	int tasks = ip_set.size();
	ApplicationContainer apps[tasks], sinkApps[tasks];
	multimap<int, string>::iterator it = ip_set.begin();	
	string sock_factory = "ns3::LinuxTcpSocketFactory";
	int i = 0;
	while(it != ip_set.end()){
		//BulkSendHelper bulk[i];
		//cout << "IP : " << (*it).second << endl;
		BulkSendHelper bulk = BulkSendHelper (sock_factory, InetSocketAddress (string2char((*it).second), 50000));
		bulk.SetAttribute ("MaxBytes", UintegerValue (size));
		apps[0] = bulk.Install (nodes.Get((*it).first));
		
		apps[0].Start(Seconds(start+0.001*i));
		apps[0].Stop(Seconds(end));
		
		PacketSinkHelper sink = PacketSinkHelper (sock_factory, InetSocketAddress (Ipv4Address::GetAny (), 50000));
		int ser = host_detect(string2char((*it).second));
		sinkApps[0] = sink.Install (nodes.Get(ser));
		sinkApps[0].Start (Seconds (start+0.001*i));
		sinkApps[0].Stop (Seconds (end));
		++it;
		i++;
	}
}

void socketTraffic2(multimap<int, string> &ip_set, int *box, int start_num, double start, double end, NodeContainer nodes, int rep_on){
	uint32_t size = 1 * 1024;
	//cout << "start:" << start_num << endl;
	int tasks = ip_set.size();
	ApplicationContainer apps[tasks], sinkApps[tasks];
	multimap<int, string>::iterator it = ip_set.begin();	
	string sock_factory = "ns3::LinuxTcpSocketFactory";
	int i = 0;
	while(it != ip_set.end()){
		size=box[start_num]*1024;
		//cout << "size:[" << i << "]" << start_num << "->" <<  size << endl;
		//BulkSendHelper bulk[i];
		//cout << "IP : " << (*it).second << endl;
		//if(box[start_num] < 1000){
			BulkSendHelper bulk = BulkSendHelper (sock_factory, InetSocketAddress (string2char((*it).second), 50000));
			bulk.SetAttribute ("MaxBytes", UintegerValue (size));
			apps[0] = bulk.Install (nodes.Get((*it).first));
			
			apps[0].Start(Seconds(start+0.001*i));
			apps[0].Stop(Seconds(end));
			
			PacketSinkHelper sink = PacketSinkHelper (sock_factory, InetSocketAddress (Ipv4Address::GetAny (), 50000));
			int ser = host_detect(string2char((*it).second));
			sinkApps[0] = sink.Install (nodes.Get(ser));
			sinkApps[0].Start (Seconds (start+0.001*i));
			sinkApps[0].Stop (Seconds (end));
		//}
		++it;
		if(rep_on){
			if(i%2)
				start_num=start_num+1;
		}
		else{
			start_num++;
		}
		i++;
	}
}

int poisson(double lam){
	//srand((unsigned) time(NULL));
    double v = exp(lam) * (double)rand()/RAND_MAX;
	int k=0;
	double rn=0;
	//rn = (double)rand()/RAND_MAX;
	//cout << "test:" << v << endl;
	while (v>=1){
		rn = (double)rand()/RAND_MAX;
		//cout << "test:" << rn << endl;
		v *= rn;
		k += 1;
	}
	return k;
}

int* poisson_pros(int sec, int end_time, int freq, int *poisson_size){
	int total_queue=0, queue=0;
	int random;
	int *pre_set;
	pre_set = new int[5000];
	int k=0;
	srand((unsigned) time(NULL));
	while(sec < end_time){
		queue = 0;
		random = rand() % 100 + 1;
		if(random < freq){
			queue++;
		}
		if(queue != 0){
			pre_set[k]=sec;
			//cout << sec << "[msec]" << queue << endl;
			k++;
		}
		sec=sec+10;
		//cout << sec << endl;
		total_queue += queue;
	}
	int *poisson_set;
	poisson_set = new int[k];
	for(int i=0;i < k;i++){
		poisson_set[i]=pre_set[i];
		cout << i << " : " << poisson_set[i] << endl;
	}
	*poisson_size = k;
	delete[] pre_set;
	return poisson_set;
}
int* poisson_pros2(int sec, int end_time, double freq, int *poisson_size){
	// 1unite 100ms
	int unite=100;
	int total_queue=0, queue=0, num=0;
	int random;
	int *pre_set;
	pre_set = new int[5000];
	int k=0;
	srand((unsigned) time(NULL));
	for (int i=0; i < (end_time-sec)/unite; i++){
		int k = poisson(freq);
		int pref_sec = sec + i*unite;
		for(int j=0;j < k; j++){
			int sec = unite * (double)rand()/RAND_MAX + pref_sec;
			pre_set[num]=sec;
			num++;
		}
	}
	int *poisson_set;
	poisson_set = new int[num];
	for(int i=0;i < num;i++){
		poisson_set[i]=pre_set[i];
		cout << i << " : " << poisson_set[i] << endl;
	}
	*poisson_size = num;
	delete[] pre_set;
	return poisson_set;
}

int* constant_pros(int sec, int end_time, int freq, int *poisson_size){
	int total_queue=0, queue=0;
	int random;
	int *pre_set;
	pre_set = new int[5000];
	int k=0;
	int interval = 10 * freq;
	srand((unsigned) time(NULL));
	while(sec < end_time){
		pre_set[k]=sec;
		k++;
		sec=sec+interval;
	}
	int *poisson_set;
	poisson_set = new int[k];
	for(int i=0;i < k;i++){
		poisson_set[i]=pre_set[i];
		cout << i << " : " << poisson_set[i] << endl;
	}
	*poisson_size = k;
	delete[] pre_set;
	return poisson_set;
}
void s_makelist (int num, multimap<int, string> &adrs_set, int rep_on){	
	int k=0;
	int er=0;
	int tmp_s, tmp_d;
	int s_src[num], s_dst[num];
	int path_r;
	double x;
	std::ostringstream cmd_oss;
	//srand((unsigned) time(NULL));
	int nodes=32;
	while(k < num){	
		er=0;
		x=((double)rand())/((double)RAND_MAX+1.0);
		tmp_s=x*nodes; 
		if(tmp_s%8==0 or tmp_s%8==1){
			er=1;	
		}
		x=((double)rand())/((double)RAND_MAX+1.0);
		tmp_d=x*nodes; 
		//cout << "[" << k  << "]src:" << tmp_s << ",dst:" << tmp_d << endl;
		if(tmp_d%8==0 or tmp_d%8==1){
			er=1;	
		}
		if(tmp_s == tmp_d){
			er=1;
		}
		if(er==0){
			//cout << "[" << k << "]src:" << tmp_s << ", dst:" << tmp_d << endl;
			cmd_oss.str("");
			x=((double)rand())/((double)RAND_MAX+1.0);
			if(rep_on){
				cmd_oss << "10." << tmp_d+1 << ".0.1";
				adrs_set.insert(map<int, string>::value_type(tmp_s, cmd_oss.str() ));
				cmd_oss.str("");
				cmd_oss << "10." << tmp_d+1 << ".1.1";
				adrs_set.insert(map<int, string>::value_type(tmp_s, cmd_oss.str() ));
			}
			else{
				path_r=x*2; //2paths
				if(path_r%2)
					cmd_oss << "10." << tmp_d+1 << ".0.1";
				else
					cmd_oss << "10." << tmp_d+1 << ".1.1";
				adrs_set.insert(map<int, string>::value_type(tmp_s, cmd_oss.str() ));
			}
			k++;
		}
	}
}

void l_makelist (int num, multimap<int, string> &adrs_set){	
	int k=0;
	int er=0;
	int num_l=8;//8 backends
	int tmp_s, tmp_d;
	int path_r;
	double x;
	std::ostringstream cmd_oss;
	//srand((unsigned) time(NULL));
	for(int i=0; i<num; i++){
		er=0;
		while(er == 0){
			if(i<4){
				tmp_s=i;
				x=((double)rand())/((double)RAND_MAX+1.0);
				tmp_d=x*num_l; 
			}
			else{
				tmp_d=i-4;
				x=((double)rand())/((double)RAND_MAX+1.0);
				tmp_s=x*num_l; 
			}
			if(tmp_s != tmp_d){
				er=1;
			}
		}
		cout << "[" << k << "]src:" << tmp_s << ", dst:" << tmp_d << endl;
		cmd_oss.str("");
		x=((double)rand())/((double)RAND_MAX+1.0);
		path_r=x*2; //2paths
		if(tmp_d>3){
			tmp_d=(tmp_d-4)*num_l+1;
		}
		else{
			tmp_d=tmp_d*num_l;	
		}
		if(tmp_s>3){
			tmp_s=(tmp_s-4)*num_l+1;
		}
		else{
			tmp_s=tmp_s*num_l;	
		}
		if(path_r%2)
			cmd_oss << "10." << tmp_d+1 << ".0.1";
		else
			cmd_oss << "10." << tmp_d+1 << ".1.1";
		adrs_set.insert(map<int, string>::value_type(tmp_s, cmd_oss.str() ));
		k++;
	}
}

void l_makelist2 (int num, multimap<int, string> &adrs_set){	
	int k=0;
	int er=0;
	int num_l=8;//8 backends
	int tmp_s, tmp_d;
	int path_r;
	double x;
	std::ostringstream cmd_oss;
	//srand((unsigned) time(NULL));
	for(int i=0; i<num; i++){
		er=0;
		while(er == 0){
			er=0;
			tmp_s=i;
			x=((double)rand())/((double)RAND_MAX+1.0);
			tmp_d=x*num_l; 
			//cout << "tmp_d: " << tmp_d << "tmp_s: " << tmp_s << endl;
			if(tmp_s != tmp_d){
				er=1;
			}
		}
		//cout << "[" << k << "]src:" << tmp_s << ", dst:" << tmp_d << endl;
		cmd_oss.str("");
		x=((double)rand())/((double)RAND_MAX+1.0);
		path_r=x*2; //2paths
		if(tmp_d>3){
			tmp_d=(tmp_d-4)*num_l+1;
		}
		else{
			tmp_d=tmp_d*num_l;	
		}
		if(tmp_s>3){
			tmp_s=(tmp_s-4)*num_l+1;
		}
		else{
			tmp_s=tmp_s*num_l;	
		}
		if(path_r%2)
			cmd_oss << "10." << tmp_d+1 << ".0.1";
		else
			cmd_oss << "10." << tmp_d+1 << ".1.1";
		adrs_set.insert(map<int, string>::value_type(tmp_s, cmd_oss.str() ));
		k++;
	}
}
void l_makelist3 (int num, multimap<int, string> &adrs_set){	
	int k=0;
	int er=0;
	int num_l=8;//8 backends
	int tmp_s, tmp_d;
	int path_r;
	double x;
	std::ostringstream cmd_oss;
	//srand((unsigned) time(NULL));
	for(int i=num_l-1; i>=num_l-num; i--){
		cout << "i: " << i << endl;
		er=0;
		while(er == 0){
			er=0;
			tmp_s=i;
			x=((double)rand())/((double)RAND_MAX+1.0);
			tmp_d=x*num_l; 
			//cout << "tmp_d: " << tmp_d << "tmp_s: " << tmp_s << endl;
			if(tmp_s != tmp_d){
				er=1;
			}
		}
		cout << "[" << k << "]src:" << tmp_s << ", dst:" << tmp_d << endl;
		cmd_oss.str("");
		x=((double)rand())/((double)RAND_MAX+1.0);
		path_r=x*2; //2paths
		if(tmp_d>3){
			tmp_d=(tmp_d-4)*num_l+1;
		}
		else{
			tmp_d=tmp_d*num_l;	
		}
		if(tmp_s>3){
			tmp_s=(tmp_s-4)*num_l+1;
		}
		else{
			tmp_s=tmp_s*num_l;	
		}
		if(path_r%2)
			cmd_oss << "10." << tmp_d+1 << ".0.1";
		else
			cmd_oss << "10." << tmp_d+1 << ".1.1";
		adrs_set.insert(map<int, string>::value_type(tmp_s, cmd_oss.str() ));
		k++;
	}
}
double pareto(double a, int mu, double x){
	double b = mu * (a-1) / a;
	//cout << "b:" << b << endl;
	//double ab = a / b;
	double denomi = pow(1-x, 1/a);
	//cout << "denomi:" << denomi << endl;
	double result = b / denomi;
	return result;
}
void pareto_make(double a, double mu, int num, int *flow_box){
	//flow_box = new int[5000];
	int tmp=0, max=0, cnt=0;
	double x;
	srand((unsigned) time(NULL));
	for(int i=0; i<num; i++){
		x = (double)rand()/RAND_MAX;
		tmp=pareto(a, mu, x);
		if(tmp > max){
			max = tmp;
		}
		flow_box[i]=tmp;
		cnt++;
	}
	//cout << "max_const:" << max << endl;
}

void setPos (Ptr<Node> n, int x, int y, int z)
{
  Ptr<ConstantPositionMobilityModel> loc = CreateObject<ConstantPositionMobilityModel> ();
  n->AggregateObject (loc);
  Vector locVec2 (x, y, z);
  loc->SetPosition (locVec2);
}

int main (int argc, char *argv[])
{
  uint32_t nRtrs = 4;
  uint32_t nNodes = 40;
  uint32_t s_nodes = 30;
  uint32_t s_size = 100; 
  uint32_t nDir_name = 1;
  double end = 10.0;
  int rep_tcp = 0; //on:1, off:0
  int rep_which =0 ; //rtr->0, rtr->1
  int load=20;

  CommandLine cmd;
  cmd.AddValue ("nRtrs", "Number of routers. Default 2", nRtrs);
  cmd.AddValue ("nNodes", "Number of routers. Default 3", nNodes);
  cmd.AddValue ("s_nodes", "Number of sender. Default nNodes-1", s_nodes);
  cmd.AddValue ("nDir", "the place of pcap files", nDir_name);
  cmd.AddValue ("s_size", "Traffic size in short flow", s_size);
  cmd.AddValue ("rep_which", "which interface for shortflow", rep_which);
  cmd.AddValue ("load", "what load value is", load);
  cmd.Parse (argc, argv);

  NodeContainer nodes, routers, routers_aggr;
  //int nodes_num=nNodes * 2;
  int pod=8;
  int nodes_num=16*2;
  int rtrs_num=nRtrs*8;
  nodes.Create (nodes_num);
  routers.Create (rtrs_num);
  routers_aggr.Create (nRtrs);

  DceManagerHelper dceManager;
  dceManager.SetTaskManagerAttribute ("FiberManagerType",
                                      StringValue ("UcontextFiberManager"));

  dceManager.SetNetworkStack ("ns3::LinuxSocketFdFactory",
                              "Library", StringValue ("liblinux.so"));
  LinuxStackHelper stack;
  stack.Install (nodes);
  stack.Install (routers);
  stack.Install (routers_aggr);

  dceManager.Install (nodes);
  dceManager.Install (routers);
  dceManager.Install (routers_aggr);

  PointToPointHelper pointToPoint;
  NetDeviceContainer devices[nodes_num+2];
  Ipv4AddressHelper address_list[nodes_num+2];
  std::ostringstream cmd_oss;
  for (int j=0; j < nodes_num+2; j++){
    cmd_oss.str ("");
    cmd_oss << "10." << j+1 << ".0.0";
  	address_list[j].SetBase (cmd_oss.str().c_str(), "255.255.255.0");
  }
  Ipv4InterfaceContainer ifc[nodes_num+2];

  for (uint32_t i = 0; i < nRtrs; i++)
    {
      // Left link
      pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
      pointToPoint.SetChannelAttribute ("Delay", StringValue ("1ns"));
	  for (int j=0; j < nodes_num/2; j++){
	  	//cout << "test:" << j*2 << "<->" << j*2 /pod *(nRtrs*2)  + i*2<< endl;
	  	devices[j*2] = pointToPoint.Install (nodes.Get (j*2), routers.Get (j*2 /pod *(nRtrs*2) + i*2));
        ifc[j*2] = address_list[j*2].Assign (devices[j*2]);
      	address_list[j*2].NewNetwork ();
		if(rep_tcp==0 or (i==0 and (j<8 or(j>7 and j%2 == 1))) or (i==1 and (j<8 or (j>7 and j%2 == 0)))){
			//cout << "node" << j*2 << ", i:" << i << endl;
			for (int num_k=0; num_k<nodes_num;num_k++){
				if(num_k != j*2){
        			cmd_oss.str ("");
        			cmd_oss << "route add 10." << num_k+1 <<"." << i << ".0/24 dev sim" << i;
					//cout << cmd_oss.str ().c_str () << endl;
        			LinuxStackHelper::RunIp (nodes.Get (j*2), Seconds (0.1), cmd_oss.str ().c_str ());
				}
			}
		}
	  	//cout << endl;

	  }
	  //cout << endl;

      // middle link
	  if(i==0){
      	pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
      	//pointToPoint.SetChannelAttribute ("Delay", StringValue ("1ns"));
	  	pointToPoint.SetChannelAttribute ("Delay", TimeValue(MicroSeconds(25)));
      	//pointToPoint.SetChannelAttribute ("Delay", StringValue ("1ms"));
	  }else{
      	pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
	  	pointToPoint.SetChannelAttribute ("Delay", TimeValue(MicroSeconds(25)));
      	//pointToPoint.SetChannelAttribute ("Delay", StringValue ("10ms"));
	  }
	  // // edge_aggr_left
	  for (int j=0; j < rtrs_num/2 / nRtrs; j++){
	  	//cout << "test:" << j << endl;
      	devices[nodes_num] = pointToPoint.Install (routers_aggr.Get (i), routers.Get (j*nRtrs*2 + i*2));
      	// Assign ip addresses
      	ifc[nodes_num] = address_list[nodes_num].Assign (devices[nodes_num]);
      	address_list[nodes_num].NewNetwork ();
      	// setup ip routes
	  	
		//cout << "routers_aggr:" << i << ",router:" << j*nRtrs*2 + i*2 << endl;
	  	for (int l=0; l < nodes_num/2; l++){
      		cmd_oss.str ("");
      		cmd_oss << "route add 10." << (l+1)*2  << ".0.0/16 via " << ifc[nodes_num].GetAddress (0, 0) ;
      		LinuxStackHelper::RunIp (routers.Get (j*nRtrs*2 + i*2), Seconds (0.2), cmd_oss.str ().c_str ());
	  	    //cout << cmd_oss.str ().c_str () << endl;
      		cmd_oss.str ("");
			//cout << (l*2+1)/8 << "vs" << (j*nRtrs*2+i*2)/8 <<endl;
			//if((l*2+1)/(nRtrs*2) ==  (j*nRtrs*2+i*2)/(nRtrs*2)){
			if(j == (l*2+1)/pod ){
				//cout << "hitttt" << endl;
      			cmd_oss << "route add 10." << l*2+1 << ".0.0/16 via " << ifc[nodes_num].GetAddress (1, 0) ;
      			LinuxStackHelper::RunIp (routers_aggr.Get (i), Seconds (0.1), cmd_oss.str ().c_str ());
			}
			else{
      			cmd_oss << "route add 10." << l*2+1 << ".0.0/16 via " << ifc[nodes_num].GetAddress (0, 0) ;
      			LinuxStackHelper::RunIp (routers.Get (j*nRtrs*2 + i*2), Seconds (0.2), cmd_oss.str ().c_str ());
			}
	  	    //cout << cmd_oss.str ().c_str () << endl;
      	}
	  }
	  // // edge_aggr_right
	  for (int j=0; j < rtrs_num/2/nRtrs; j++){
      	devices[nodes_num+1] = pointToPoint.Install (routers_aggr.Get (i), routers.Get (j*nRtrs*2 + i*2 + 1));
      	// Assign ip addresses
      	ifc[nodes_num+1] = address_list[nodes_num+1].Assign (devices[nodes_num+1]);
      	address_list[nodes_num+1].NewNetwork ();
      	// setup ip routes
	  	//cout << "test2:" << j << endl;
		//cout << "routers_aggr:" << i << ",router:" << (j*nRtrs*2+i*2)/2 << endl;
	  	for (int l=0; l < nodes_num/2; l++){
      		cmd_oss.str ("");
	  		//cout << "test2:" << j << endl;
			//cout << (l*2)/8 << "vs" << (j*nRtrs*2+i*2)/8 <<endl;
			//if(l*2/(nRtrs*2)==  (j*nRtrs*2+i*2)/(nRtrs*2)){
			if(j == (l+1)*2/pod){
				//cout << "hitttt2" << endl;
      			cmd_oss << "route add 10." << (l+1)*2  << ".0.0/16 via " << ifc[nodes_num+1].GetAddress (1, 0) ;
      			LinuxStackHelper::RunIp (routers_aggr.Get (i), Seconds (0.2), cmd_oss.str ().c_str ());
			}
			else{
      			cmd_oss << "route add 10." << (l+1)*2  << ".0.0/16 via " << ifc[nodes_num+1].GetAddress (0, 0) ;
      			LinuxStackHelper::RunIp (routers.Get (j*nRtrs*2+i*2+1), Seconds (0.1), cmd_oss.str ().c_str ());
			}
	  	  	//cout << cmd_oss.str ().c_str () << endl;
      		cmd_oss.str ("");
      		cmd_oss << "route add 10." << l*2+1 << ".0.0/16 via " << ifc[nodes_num+1].GetAddress (0, 0) ;
      		LinuxStackHelper::RunIp (routers.Get (j*nRtrs*2+i*2+1), Seconds (0.2), cmd_oss.str ().c_str ());
	  	  	//cout << cmd_oss.str ().c_str () << endl;
      	}
	  }
	  //cout << endl;
      // Right link
      pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
      pointToPoint.SetChannelAttribute ("Delay", StringValue ("1ns"));
	  for (int j=0; j < nodes_num/2; j++){
	  	devices[j*2+1] = pointToPoint.Install (nodes.Get (j*2+1), routers.Get ((j*2+1)/pod*(nRtrs*2)+i*2+1));
        ifc[j*2+1] = address_list[j*2+1].Assign (devices[j*2+1]);
      	address_list[j*2+1].NewNetwork ();
		if(rep_tcp==0 or (i==0 and (j<8 or(j>7 and j%2 == 1))) or (i==1 and (j<8 or (j>7 and j%2 == 0)))){
			//cout << "node" << j*2+1 << ", i:" << i << endl;
			for (int num_k=0; num_k < nodes_num; num_k++){
				if(num_k != j*2+1){	
        			cmd_oss.str ("");
        			cmd_oss << "route add 10." << num_k+1 <<"." << i << ".0/24 dev sim" << i;
					//cout << cmd_oss.str ().c_str () << endl;
        			LinuxStackHelper::RunIp (nodes.Get (j*2+1), Seconds (0.1), cmd_oss.str ().c_str ());
				}
			}
		}
	  //cout << endl;
	  }
	  //cout << endl;

      setPos (routers_aggr.Get (i), 50, i * 20, 0);
    }

  // default route
  
  for (int j=0; j < nodes_num; j++){	
        cmd_oss.str ("");
        cmd_oss << "route add default via 10." << j+1 << ".1.2 dev sim1";
		/*
		if((j/2)%2)
        	cmd_oss << "route add default via 10." << j+1 << ".1.2 dev sim1";
		else
        	cmd_oss << "route add default via 10." << j+1 << ".0.2 dev sim0";
		*/
		//cout << cmd_oss.str ().c_str () << endl;
  		//LinuxStackHelper::RunIp (nodes.Get (j), Seconds (0.1), cmd_oss.str ().c_str ());
  }
  
  /*
  cmd_oss << "route add default via 10." << 4 << ".1.2 dev sim1";
  cout << cmd_oss.str ().c_str () << endl;
  LinuxStackHelper::RunIp (nodes.Get (3), Seconds (0.1), cmd_oss.str ().c_str ());
  */
  /*
  LinuxStackHelper::RunIp (nodes.Get (0), Seconds (0.1), "route add default via 10.1.0.2 dev sim0");
  LinuxStackHelper::RunIp (nodes.Get (1), Seconds (0.1), "route add default via 10.2.0.2 dev sim0");
  LinuxStackHelper::RunIp (nodes.Get (2), Seconds (0.1), "route add default via 10.3.1.2 dev sim1");
  LinuxStackHelper::RunIp (nodes.Get (3), Seconds (0.1), "route add default via 10.4.1.2 dev sim1");
  LinuxStackHelper::RunIp (nodes.Get (4), Seconds (0.1), "route add default via 10.5.0.2 dev sim0");
  LinuxStackHelper::RunIp (nodes.Get (5), Seconds (0.1), "route add default via 10.6.0.2 dev sim0");
  */

  for (int i=0; i < nRtrs; i++){	
  	for (int j=0; j < nodes_num+1; j++){	
  	  cmd_oss.str ("");
  	  cmd_oss << "link set sim" << j << " txqueue 100";
	  LinuxStackHelper::RunIp (routers_aggr.Get (i), Seconds (0.1), cmd_oss.str().c_str ());
	}
	LinuxStackHelper::RunIp (routers_aggr.Get (i), Seconds (0.1), "addr show");
  }
  for (int i=0; i < rtrs_num; i++){	
  	for (int j=0; j < nodes_num+1; j++){	
  	  cmd_oss.str ("");
  	  cmd_oss << "link set sim" << j << " txqueue 100";
	  LinuxStackHelper::RunIp (routers.Get (i), Seconds (0.1), cmd_oss.str().c_str ());
	}
	LinuxStackHelper::RunIp (routers.Get (i), Seconds (0.1), "addr show");
  }
  stack.SysctlSet (nodes, ".net.mptcp.mptcp_debug", "1");

  // Schedule Up/Down (XXX: didn't work...)
  /*
  LinuxStackHelper::RunIp (nodes.Get (1), Seconds (1.0), "link set dev sim0 multipath off");
  LinuxStackHelper::RunIp (nodes.Get (1), Seconds (15.0), "link set dev sim0 multipath on");
  LinuxStackHelper::RunIp (nodes.Get (1), Seconds (30.0), "link set dev sim0 multipath off");
  */

  // debug
  stack.SysctlSet (nodes, ".net.mptcp.mptcp_enabled", "1");
  /*
  stack.SysctlSet (nodes.Get(0), ".net.mptcp.mptcp_enabled", "0");
  stack.SysctlSet (nodes.Get(8), ".net.mptcp.mptcp_enabled", "0");
  stack.SysctlSet (nodes.Get(16), ".net.mptcp.mptcp_enabled", "0");
  stack.SysctlSet (nodes.Get(24), ".net.mptcp.mptcp_enabled", "0");
  stack.SysctlSet (nodes.Get(1), ".net.mptcp.mptcp_enabled", "0");
  stack.SysctlSet (nodes.Get(9), ".net.mptcp.mptcp_enabled", "0");
  stack.SysctlSet (nodes.Get(17), ".net.mptcp.mptcp_enabled", "0");
  stack.SysctlSet (nodes.Get(25), ".net.mptcp.mptcp_enabled", "0");
  */

  stack.SysctlSet(nodes, ".net.ipv4.tcp_rmem", "5000000 5000000 5000000");
  stack.SysctlSet(nodes, ".net.ipv4.tcp_wmem", "5000000 5000000 5000000");
  stack.SysctlSet(nodes, ".net.core.rmem_max", "5000000");
  stack.SysctlSet(nodes, ".net.core.wmem_max", "5000000");
  DceApplicationHelper dce;
  ApplicationContainer apps;

  dce.SetStackSize (1 << 20);

  
  string pcap_place = "./pcap/iperf-mptcp_fattree/original/" + int2string(nDir_name)  + "/iperf-mptcp";
  pointToPoint.EnablePcapAll (pcap_place, false);

  int sum_tr = nNodes;
  multimap<int, string> adrs_set, adrs_set2, adrs_set3, adrs_set4;
  multimap<string, string> adrs_test;
  /*
  for (int j=1;j < sum_tr; j++){
  	adrs_set.insert(map<int, string>::value_type(j*2, "10.4.1.1"));
  }
  */
  for (int j=0;j < s_nodes; j++){
  	int base_num=17;
    cmd_oss.str ("");
	/*
	if(j % 2)
    	cmd_oss << "10." << j*2+2 << ".1.1";
	else	
    	cmd_oss << "10." << j*2+2 << ".0.1";
	*/
	if(j%2){
    	cmd_oss << "10." << 1 << ".0.1";
	}else{
    	cmd_oss << "10." << 3 << ".1.1";
	}
  	adrs_set.insert(map<int, string>::value_type(j*2+base_num, cmd_oss.str().c_str ()));
  }
  //adrs_set.insert(map<int, string>::value_type(2, "10.4.1.1"));
  //adrs_set.insert(map<int, string>::value_type(4, "10.6.0.1"));
  //adrs_test.insert(map<string, string>::value_type("10.1.1.1", "10.2.1.1"));
  //adrs_set2.insert(map<int, string>::value_type(2, "10.3.0.1"));
  adrs_set3.insert(map<int, string>::value_type(5, "10.5.0.1"));
  adrs_set3.insert(map<int, string>::value_type(7, "10.7.0.1"));
  adrs_set3.insert(map<int, string>::value_type(9, "10.9.0.1"));
  adrs_set3.insert(map<int, string>::value_type(11, "10.11.0.1"));
  adrs_set3.insert(map<int, string>::value_type(13, "10.13.0.1"));
  adrs_set3.insert(map<int, string>::value_type(15, "10.15.0.1"));
  adrs_set4.insert(map<int, string>::value_type(5, "10.5.1.1"));
  adrs_set4.insert(map<int, string>::value_type(7, "10.7.1.1"));
  adrs_set4.insert(map<int, string>::value_type(9, "10.9.1.1"));
  adrs_set4.insert(map<int, string>::value_type(11, "10.11.1.1"));
  adrs_set4.insert(map<int, string>::value_type(13, "10.13.1.1"));
  adrs_set4.insert(map<int, string>::value_type(15, "10.15.1.1"));
  //adrs_set4.insert(map<int, string>::value_type(4, "10.2.0.1"));
  /*
  cout << "-- adrs_set --" << endl;
  show_multimap(adrs_set);
  //show_multimap(adrs_set2);
  cout << "-- adrs_set3 --" << endl;
  show_multimap(adrs_set3);
  cout << "-- adrs_set4 --" << endl;
  show_multimap(adrs_set4);
  */
  int s_num, l_num;

  multimap<int, string> short_list, long_list;
  s_num=24*load/100;
  cout << "short_num" << s_num << endl;
  l_num=8*load/100;
  cout << "long_num" << l_num << endl;
  int poisson_size, constant_size, constant_size2, constant_size3;
  //int *poisson_set = poisson_pros2(3500, 6000, 0.5, &poisson_size);
  int *constant_set = constant_pros(3600, (end-2)*1000, 20, &constant_size);
  int *constant_set2 = constant_pros(3600, (end-2)*1000, 100, &constant_size2);
  int *constant_set3 = constant_pros(3650, (end-2)*1000, 100, &constant_size2);

  int num=constant_size*s_num;
  int *short_box=new int[num];
  pareto_make(1.5, 50, num, short_box);
  cout << "num:" << num << endl;
  num=constant_size2*l_num;
  int *long_box=new int[num];
  pareto_make(1.2, 10000, num, long_box);
  cout << "num:" << num << endl;
  
  for(int i=0; i<num; i++){
  	cout << long_box[i] << endl;
  }
  
  //rep_which=0; 
  int cnt_flow=0;
  int rep_on=0;
  for(int i=0; i<constant_size;i++){
  	s_makelist(s_num, short_list, rep_on);
  	//l_makelist(l_num, long_list);
    socketTraffic2(short_list, short_box, cnt_flow, (double) constant_set[i] / 1000, end, nodes, rep_on);
	cnt_flow = cnt_flow + (int)short_list.size()/(rep_on+1) ;
  	//show_multimap(short_list);
  	short_list.clear();
  }
  l_makelist2(l_num, long_list);
  //l_makelist3(l_num, long_list);
  show_multimap(long_list);
  //socketTraffic(long_list, l_size, 4.0, end, nodes);
  cnt_flow=0;
  for(int i=0; i<constant_size2;i++){
  	//s_makelist(s_num, short_list, rep_on);
  	//l_makelist(l_num, long_list);
    socketTraffic2(long_list, long_box, cnt_flow, (double) constant_set2[i] / 1000, end, nodes, 0);
    //socketTraffic(long_list, 10, (double) constant_set2[i] / 1000, end, nodes);
	cnt_flow = cnt_flow + (int)long_list.size();
  }

  //socketTraffic(adrs_set, 0, 4.0, 6.0, nodes); 
  //socketTraffic(adrs_set3, 70, 4.0, 6.0, nodes); 
  //adrs_set2.insert(map<int, string>::value_type(19, "10.1.0.1"));
  //socketTraffic(adrs_set2, 76000, 4.0, end, nodes); 
  //adrs_set2.clear();
  //socketTraffic(adrs_set, s_size, 3.5, 6.0, nodes);
  //socketTraffic(adrs_set4, 0, 4.0, end, nodes);
  //setPos (routers.Get (i), 30, i * 20, 0);
  for (int j=0; j < nodes_num/2; j++){
  	if(nodes_num % 4 != 0){
  			setPos (nodes.Get (j*2), 0, 20 * (nRtrs - 1) / 2 +(nodes_num/4 - j)*20, 0);
  			setPos (nodes.Get (j*2+1), 100, 20 * (nRtrs - 1) / 2 +(nodes_num/4 - j)*20, 0);
	}else{
  			setPos (nodes.Get (j*2), 0, 20 * (nRtrs - 1) / 2 +(nodes_num/4 - j)*20 - 10, 0);
  			setPos (nodes.Get (j*2+1), 100, 20 * (nRtrs - 1) / 2 +(nodes_num/4 - j)*20 - 10, 0);
	}
  }
  for (int j=0; j < rtrs_num/2; j++){
  	if(rtrs_num % 4 != 0){
  			setPos (routers.Get (j*2), 25, 20 * (nRtrs - 1) / 2 +(rtrs_num/4 - j)*20, 0);
  			setPos (routers.Get (j*2+1), 75, 20 * (nRtrs - 1) / 2 +(rtrs_num/4 - j)*20, 0);
	}else{
  			setPos (routers.Get (j*2), 25, 20 * (nRtrs - 1) / 2 +(rtrs_num/4 - j)*20 - 10, 0);
  			setPos (routers.Get (j*2+1), 75, 20 * (nRtrs - 1) / 2 +(rtrs_num/4 - j)*20 - 10, 0);
	}
  }
  /*
  setPos (nodes.Get (0), 0, 20 * (nRtrs - 1) / 2 + 20, 0);
  setPos (nodes.Get (1), 100, 20 * (nRtrs - 1) / 2 + 20, 0);
  setPos (nodes.Get (2), 0, 20 * (nRtrs - 1) / 2 , 0);
  setPos (nodes.Get (3), 100, 20 * (nRtrs - 1) / 2 , 0);
  setPos (nodes.Get (4), 0, 20 * (nRtrs - 1) / 2 - 20, 0);
  setPos (nodes.Get (5), 100, 20 * (nRtrs - 1) / 2 - 20, 0);
  */

  Simulator::Stop (Seconds (end));
  //AnimationInterface anim("./xml/dce-iperf-mptcp_fattree_mining.xml");
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
