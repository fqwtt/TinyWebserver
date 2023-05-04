/*************************************************************************
  > File Name: config.h
  > Author: fqwtt
  > Created Time: 2023年04月27日 星期四 10时58分51秒
 ************************************************************************/
#ifndef CONFIG_H
#define CONFIG_H
#include <iostream>
#include "webserver.h>"

using namespace std;

class Config{
	public:
		Config();
		~Config();
		void parase_arg(int argc, char* argv[]);

		//端口号
		int Port;

		//日志写入方式
		int LogWrite;

		//触发组合模式
		int TrigMode;

		//listenfd触发组合模式
		int ListenTrigMode;

		//connfd触发模式
		int ConnTrigMode;

		//优雅关闭链接
		int OptLinger;

		//数据库连接池数量
		int SqlNum;

		//线程池内的线程数量
		int ThreadNum;

		//是否关闭日志
		int CloseLog;

		//并法模型选择
		int ActorModel;
};

#endif
