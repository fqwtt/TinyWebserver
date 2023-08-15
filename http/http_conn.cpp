/*************************************************************************
  > File Name: http_conn.cpp
  > Author: fqwtt
  > Created Time: 2023年05月31日 星期三 9时19分33秒
 ************************************************************************/
#include <fstream>

#include "http_conn.h"

const char* ok_200_title = "OK";
const char* error_400_title = "Bad Request";
const char* error_400_form = "Your request has bad syntax or is inherently impossible to staisfy.\n";
const char* error_403_title = "Forbidden";
const char* error_403_form = "You do not have permission to get file form this server.\n";
const char* error_404_title = "Not Found";
const char* error_404_form = "The requested file was not found on this server.\n";
const char* error_500_title = "Internal Error";
const char* error_500_form = "There was an unusual problem serving the request file.\n";

std::mutex m_mtx;
std::map<std::string, std::string> users;

void http_conn::initmysql_result(connection_pool* connPool) {
  MYSQL* mysql = NULL;
  connectionRAII mysqlcon(&mysql, connPool);
  if (mysql_query(mysql, "SELECT username, passwd FROM user")) {
    printf("SELECT error:%s\n", mysql_error(mysql));  // 后续修改
    return;
  }
  MYSQL_RES* result = mysql_store_result(mysql);
  int num_fileds = mysql_num_fields(result);
  MYSQL_FIELD* fields = mysql_fetch_fields(result);
  while (MYSQL_ROW row = mysql_fetch_row(result)) {
    string user(row[0]);
    string passwd(row[1]);
    users[user] = passwd;
  }
}

// 对文件描述符设置非阻塞
int setnonblocking(int fd) {
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);
  return old_option;
}

// 将内核事件表注册读事件，ET模式，选择开启EPOOONSHOT
void addfd(int epollfd, int fd, bool one_shot, int TRIGMode) {
  epoll_event event;
  event.data.fd = fd;
  if (1 == TRIGMode) {
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
  } else {
    event.events = EPOLLIN | EPOLLRDHUP;
  }
  if (one_shot) event.events |= EPOLLONESHOT;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
  setnonblocking(fd);
}

// 从内核事件表删除描述符
void removefd(int epollfd, int fd) {
  epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
  close(fd);
}

// 将事件重置为EPOLLONSHOT
void modfd(int epollfd, int fd, int ev, int TRIGMode) {
  epoll_event event;
  event.data.fd = fd;
  if (1 == TRIGMode) {
    event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
  } else {
    event.events = ev | EPOLLONESHOT | EPOLLRDHUP;
  }
  epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

int http_conn::m_user_count = 0;
int http_conn::m_epollfd = -1;

//关闭连接，每关闭一个连接，客户总量减一
void http_conn::close_conn(bool real_close) {
  if (real_close && (m_sockfd != 1)) {
    printf("close %d", m_sockfd);
    removefd(m_epollfd, m_sockfd);
    m_sockfd -= 1;
    m_user_count--;
  }
}

//初始化连接，外部调用初始化套接字地址
void http_conn::init(int sockfd, const sockaddr_in& addr, char* root, int TRIGMode, int close_log, string user,
                     string passwd, string sqlname) {
  m_sockfd = sockfd;
  m_address = addr;
  addfd(m_epollfd, sockfd, true, m_TRIGMode);
  m_user_count++;

  //当浏览器出现连接重置时，可能是网站根目录出错或http响应格式出错或者访问的文件中内容完全为空
  doc_root = root;
  m_TRIGMode = TRIGMode;
  m_close_log = close_log;
  strcpy(sql_user, user.c_str());
  strcpy(sql_passwd, passwd.c_str());
  strcpy(sql_name, sqlname.c_str());

  init();
}

//初始化新接受的连接
// check_state默认为分析请求行状态
void http_conn::init() {
  mysql = NULL;
  bytes_to_send = 0;
  bytes_have_send = 0;
  m_check_state = CHECK_STATE::CHECK_STATE_REQUESTLINE;
  m_linger = false;
  m_method = METHOD::GET;
  m_url = 0;
  m_version = 0;
  m_content_length = 0;
  m_host = 0;
  m_start_line = 0;
  m_checked_idx = 0;
  m_read_idx = 0;
  m_write_idx = 0;
  cgi = 0;
  m_state = 0;
  timer_flag = 0;
  improv = 0;

  memset(m_read_buf, '\0', READ_BUFFER_SIZE);
  memset(m_write_buf, '\0', WRITE_BUFFER_SIZE);
  memset(m_real_file, '\0', FILENAME_LEN);
}

//从状态机，用于分析出一行内容
//返回值为行的读取状态，有LINE_OK,LINE_BAD,LINE_OPEN