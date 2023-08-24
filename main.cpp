/*************************************************************************
  > File Name: main.cpp
  > Author: fqwtt
  > Created Time: 2023年04月27日 星期四 10时56分21秒
 ************************************************************************/

#include "config.h"
using namespace std;

int main(int argc, char* argv[]) {
  string user = "root";
  string passwd = "P@ssw0rd";
  string databasename = "test";

  // 命令解析
  Config config;
  config.parase_arg(argc, argv);

  WebServer server;

  // 初始化
  server.init(config.Port, user, passwd, databasename, config.LogWrite, config.OptLinger, config.TrigMode,
              config.SqlNum, config.ThreadNum, config.CloseLog, config.ActorModel);

  // 日志
  server.logWrite();

  // 数据库
  server.sqlPool();

  // 线程池
  server.threadPool();

  // 触发模式
  server.trigMode();
  
  // 监听
  server.eventListen();

  // 运行
  server.eventLoop();

  return 0;
}