/*************************************************************************
  > File Name: webserver.cpp
  > Author: fqwtt
  > Created Time: 2023年05月30日 星期二 21时46分25秒
 ************************************************************************/

#include "webserver.h"

#include <iostream>

using namespace std;

WebServer::WebServer() {
  users = new http_conn[MAX_FD];

  // root文件夹路径
  char server_path[200];
  getcwd(server_path, 200);
  char root[6] = "/root";
  m_root = (char*)malloc(strlen(server_path) + strlen(root) + 1);
  strcpy(m_root, server_path);
  strcat(m_root, root);
}

WebServer::~WebServer() {
  close(m_epollfd);
  close(m_listenfd);
  close(m_pipefd[1]);
  close(m_pipefd[0]);
  delete[] users;
  delete users_timer;
  delete m_pool;
}

void WebServer::init(int port, string user, string passWord, string databaseName, int logWrite, int optLinger,
                     int trigMode, int sqlNum, int threadNum, int closeLog, int actorModel) {
  m_port = port;
  m_user = user;
  m_passWord = passWord;
  m_databaseName = databaseName;
  m_sql_num = sqlNum;
  m_thread_num = threadNum;
  m_logWrite = logWrite;
  m_OPT_LINGER = optLinger;
  m_TRIGMode = trigMode;
  m_closeLog = closeLog;
  m_actorModel = actorModel;
}

void WebServer::threadPool() {
  // LT+LT
  if (0 == m_TRIGMode) {
    m_LISTENTrigmode = 0;
    m_CONNTrigmode = 0;
  }
  // LT+ET
  else if (1 == m_TRIGMode) {
    m_LISTENTrigmode = 0;
    m_CONNTrigmode = 1;
  }
  // ET+LT
  else if (2 == m_TRIGMode) {
    m_LISTENTrigmode = 1;
    m_CONNTrigmode = 0;
  }
  // ET+ET
  else if (3 == m_TRIGMode) {
    m_LISTENTrigmode = 1;
    m_CONNTrigmode = 1;
  }
}

void WebServer::sqlPool() {}

void WebServer::logWrite() {}

void WebServer::trigMode() {}

void WebServer::eventListen() {}

void WebServer::eventLoop() {}

void WebServer::timer(int connfd, sockaddr_in client_address) {}

void WebServer::adjustTimer(util_timer* timer, int sockfd) {}
