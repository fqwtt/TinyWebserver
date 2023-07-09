/*************************************************************************
  > File Name: sql_connection_pool.h
  > Author: fqwtt
  > Created Time: 2023年05月31日 星期三 10时11分08秒
 ************************************************************************/

#include <iostream>
#include <mysql/mysql.h>
#include <semaphore.h>
#include <mutex>
#include <list>

using namespace std;

class connection_pool {
public:
    MYSQL *Getconnection();

    bool ReleaseConnection(MYSQL *conn);

    int GetFreeConn();

    void DestroyPool();

    static connection_pool *GetInstance();

    void init(string url, string User, string Passward, string DataBaseName, int port, int MaxConn, int close_log);

private:
    connection_pool();

    ~connection_pool();

    int m_MaxConn;
    int m_CurrConn;
    int m_FreeConn;
    mutex m_mux;
    sem_t m_sem;
    list<MYSQL *> m_connList;

public:
    string m_url;
    string m_Port;
    string m_User;
    string m_PassWord;
    string m_DatabaseName;
    int m_close_log;
};

class connectionRAII {
public:
    connectionRAII(MYSQL **con, connection_pool *connPool);

    ~connectionRAII();

private:
    MYSQL *conRAII;
    connection_pool *poolRAII;
};

