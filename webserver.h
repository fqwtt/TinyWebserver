/*************************************************************************
  > File Name: webserver.h
  > Author: fqwtt
  > Created Time: 2023年04月27日 星期四 13时49分44秒
 ************************************************************************/
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <cassert>
#include <sys/epoll.h>
#include "./timer/lst_timer.h"
#include "./http/http_conn.h"

using namespace std;
const int MAX_FD = 65536;             //最大文件描述符
const int MAX_EVENT_NUMBER = 10000;   //最大事件数
const int TIMESLOT = 5;               //最小超时单位

class WebServer {
public:
    WebServer();

    ~WebServer();

    void init(int port, string user, string passWord, string databaseName,
              int logWrite, int optLinger, int trigMode, int sqlNum,
              int threadNum, int closeLog, int actorModel);

    void threadPool();

    void sqlPool();

    void logWrite();

    void trigMode();

    void eventListen();

    void eventLoop();

    void timer(int connfd, sockaddr_in client_address);

    void adjustTimer(util_timer *timer, int sockfd);

public:
    int m_port;
    char *m_root;
    int m_logWrite;
    int m_closeLog;
    int m_actorModel;

    int m_pipefd[2];
    int m_epollfd;
    http_conn *users;
};

#endif
