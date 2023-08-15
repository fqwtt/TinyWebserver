/*************************************************************************
  > File Name: http_conn.cpp
  > Author: fqwtt
  > Created Time: 2023年05月31日 星期三 9时19分33秒
 ************************************************************************/
#include "http_conn.h"

#include <fstream>

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
    printf("SELECT error:%s\n", mysql_error(mysql));
  }
}