/*************************************************************************
  > File Name: webserver.cpp
  > Author: fqwtt
  > Created Time: 2023年05月30日 星期二 21时46分25秒
 ************************************************************************/

#include <iostream>

#include "webserver.h"

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

void WebServer::threadPool() { m_pool = new threadpool<http_conn>(m_actorModel, m_connPool, m_thread_num); }

void WebServer::sqlPool() {
  m_connPool = connection_pool::GetInstance();
  m_connPool->init("8.146.250.215", m_user, m_passWord, m_databaseName, 3306, m_sql_num, m_closeLog);

  //初始化数据库读取表
  users->initmysql_result(m_connPool);
}

void WebServer::logWrite() {
  if (0 == m_closeLog) {
    if (1 == m_logWrite) {
      Log::get_instance()->init("./ServerLog", m_closeLog, 2000, 800000, 800);
    } else {
      Log::get_instance()->init("./ServerLog", m_closeLog, 2000, 800000, 0);
    }
  }
}

void WebServer::trigMode() {
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

void WebServer::eventListen() {
  m_listenfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(m_listenfd >= 0);

  if (0 == m_OPT_LINGER) {
    struct linger tmp = {0, 1};
    setsockopt(m_listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));
  } else if (1 == m_OPT_LINGER) {
    struct linger tmp = {1, 1};
    setsockopt(m_listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));
  }

  int ret = 0;
  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(m_port);

  int flag = 1;
  setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
  ret = bind(m_listenfd, (struct sockaddr*)&address, sizeof(address));
  assert(ret >= 0);
  ret = listen(m_listenfd, 5);
  assert(ret >= 0);

  utils.init(TIMESLOT);

  // epoll创建内核事件表
  epoll_event events[MAX_EVENT_NUMBER];
  m_epollfd = epoll_create(5);
  assert(m_epollfd != -1);

  utils.addfd(m_epollfd, m_listenfd, false, m_LISTENTrigmode);
  http_conn::m_epollfd = m_epollfd;

  ret = socketpair(PF_UNIX, SOCK_STREAM, 0, m_pipefd);
  assert(ret != -1);
  utils.setnonblocking(m_pipefd[1]);
  utils.addfd(m_epollfd, m_pipefd[0], false, 0);

  utils.addsig(SIGPIPE, SIG_IGN);
  utils.addsig(SIGALRM, utils.sig_handler, false);
  utils.addsig(SIGTERM, utils.sig_handler, false);

  alarm(TIMESLOT);

  //工具类，信号和描述符基础操作
  Utils::u_pipefd = m_pipefd;
  Utils::u_epollfd = m_epollfd;
}

void WebServer::eventLoop() {}

void WebServer::timer(int connfd, sockaddr_in client_address) {}

void WebServer::adjustTimer(util_timer* timer, int sockfd) {}
