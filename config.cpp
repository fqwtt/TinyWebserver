/*************************************************************************
  > File Name: config.cpp
  > Author: fqwtt
  > Created Time: 2023年04月27日 星期四 11时00分27秒
 ************************************************************************/

#include "config.h"

Config::Config(){
	//端口号，默认9900
	Port = 9900;

	//日志写入方式，默认同步
	LogWrite = 0;

	//触发组合模式，默认listenfd LT + connfd LT
	TrigMode = 0;

	//listenfd触发组合模式，默认LT
	ListenTrigMode = 0;

	//connfd触发模式，默认LT
	ConnTrigMode = 0;

	//优雅关闭链接，默认不使用
	OptLinger = 0;

	//数据库连接池数量，默认8
	SqlNum = 8;

	//线程池内的线程数量，默认8
	ThreadNum = 8;

	//是否关闭日志，默认不关闭
	CloseLog = 0;

	//并法模型选择，默认是proactor
	ActorModel = 0;
}

void Config::parase_arg(int argc, char* argv[]){
	int opt;
	const char* str = "p:l:m:o:s:t:c:a:";
	while((opt = getopt(argc,argv,str))!=-1){
		switch(opt){
			case 'p':
				this->Port = atoi(optarg);
				break;
			case 'l':
				this->LogWrite = atoi(optarg);
				break;
			case 'm':
				this->TrigMode = atoi(optarg);
				break;
			case 'o':
				this->OptLinger = atoi(optarg);
				break;
			case 's':
				this->SqlNum = atoi(optarg);
				break;
			case 't':
				this->ThreadNum = atoi(optarg);
				break;
			case 'c':
				this->CloseLog = atoi(optarg);
				break;
			case 'a':
				this->ActorModel = atoi(optarg);
				break;
			default:
				break;
		}
	}
}

Config::~Config(){

}
