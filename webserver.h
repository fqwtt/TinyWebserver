/*************************************************************************
  > File Name: webserver.h
  > Author: fqwtt
  > Created Time: 2023年04月27日 星期四 13时49分44秒
 ************************************************************************/
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <signal.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <iostream>

#include "./http/http_conn.h"
#include "./threadpool/threadpool.h"
#include "./timer/lst_timer.h"

using namespace std;
const int MAX_FD = 65536;            // 最大文件描述符
const int MAX_EVENT_NUMBER = 10000;  // 最大事件数
const int TIMESLOT = 5;              // 最小超时单位

class WebServer {
 public:
  WebServer();

  ~WebServer();

  void init(int port, string user, string passWord, string databaseName, int logWrite, int optLinger, int trigMode,
            int sqlNum, int threadNum, int closeLog, int actorModel);

  void threadPool();

  void sqlPool();

  void logWrite();

  void trigMode();

  void eventListen();

  void eventLoop();

  void timer(int connfd, sockaddr_in client_address);

  void adjustTimer(util_timer* timer, int sockfd);

 public:
  int m_port;
  char* m_root;
  int m_logWrite;
  int m_closeLog;
  int m_actorModel;

  int m_pipefd[2];
  int m_epollfd;
  http_conn* users;

  // 数据库相关
  connection_pool* m_connPool;
  string m_user;
  string m_passWord;
  string m_databaseName;
  int m_sql_num;

  // 线程池相关
  threadpool<http_conn>* m_pool;
  int m_thread_num;

  // epoll_event相关
  epoll_event events[MAX_EVENT_NUMBER];
  int m_listenfd;
  int m_OPT_LINGER;
  int m_TRIGMode;
  int m_LISTENTrigmode;
  int m_CONNTrigmode;

  // 定时器相关
  client_data* users_timer;
  Utils utils;
};

#endif
