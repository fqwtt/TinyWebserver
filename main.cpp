/*************************************************************************
  > File Name: main.cpp
  > Author: fqwtt
  > Created Time: 2023年04月27日 星期四 10时56分21秒
 ************************************************************************/

#include <iostream>
#include <unistd.h>
#include "config.h"
using namespace std;

int main(int argc,char *argv[]){
	//Config config;
	//config.parse_arg(argc,argv);
	//cout<<config.PORT<<endl;
	cout<<argc<<endl;
	int opt;
	while(true){
		opt=getopt(argc,argv,"a:b:c:d:efg");
		if (opt==-1){
			break;
		}
		cout<<optarg<<endl;
	}
	return 0;
}
