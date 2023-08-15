/*************************************************************************
  > File Name: sql_connection_pool.cpp
  > Author: fqwtt
  > Created Time: 2023年05月31日 星期三 10时11分17秒
 ************************************************************************/

#include <iostream>

#include "sql_connection_pool.h"

using namespace std;

connection_pool::connection_pool() {
  this->m_CurrConn = 0;
  this->m_FreeConn = 0;
}

connection_pool::~connection_pool() { DestroyPool(); }

MYSQL* connection_pool::Getconnection() {
  MYSQL* con = nullptr;
  if (this->m_connList.empty()) return nullptr;
  sem_wait(&this->m_sem);
  this->m_mux.lock();
  con = this->m_connList.front();
  this->m_connList.pop_front();
  this->m_FreeConn--;
  this->m_CurrConn++;
  this->m_mux.unlock();
  return con;
}

bool connection_pool::ReleaseConnection(MYSQL* conn) {
  if (conn == nullptr) return false;
  this->m_mux.lock();
  this->m_connList.push_back(conn);
  this->m_FreeConn++;
  this->m_CurrConn--;
  this->m_mux.unlock();
  sem_post(&this->m_sem);
  return true;
}

int connection_pool::GetFreeConn() { return this->m_FreeConn; }

void connection_pool::DestroyPool() {
  this->m_mux.lock();
  if (!this->m_connList.empty()) {
    for (auto& it : this->m_connList) {
      mysql_close(it);
    }
    this->m_CurrConn = 0;
    this->m_FreeConn = 0;
    this->m_connList.clear();
  }
  this->m_mux.unlock();
}

connection_pool* connection_pool::GetInstance() {
  static connection_pool connPool;
  return &connPool;
}

void connection_pool::init(string host, string User, string Passward,
                           string DataBaseName, int port, int MaxConn,
                           int close_log) {
  this->m_host = host;
  this->m_User = User;
  this->m_PassWord = Passward;
  this->m_DatabaseName = DataBaseName;
  this->m_Port = to_string(port);
  this->m_MaxConn = MaxConn;
  this->m_close_log = close_log;
  for (int i = 0; i < MaxConn; i++) {
    MYSQL* conn = NULL;
    conn = mysql_init(conn);
    if (!conn) {
      cout << "error" << endl;  // 需要修改
      exit(1);
    }
    conn =
        mysql_real_connect(conn, host.c_str(), User.c_str(), Passward.c_str(),
                           DataBaseName.c_str(), port, nullptr, 0);
    if (!conn) {
      cout << "error" << endl;  // 需要修改
      exit(1);
    }
    this->m_connList.push_back(conn);
  }
  sem_post(&this->m_sem);
  this->m_MaxConn = this->m_FreeConn;
}

connectionRAII::connectionRAII(MYSQL** con, connection_pool* connPool) {
  *con = connPool->Getconnection();
  this->conRAII = *con;
  this->poolRAII = connPool;
}

connectionRAII::~connectionRAII() {
  this->poolRAII->ReleaseConnection(this->conRAII);
}
