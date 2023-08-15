/*************************************************************************
  > File Name: main.cpp
  > Author: fqwtt
  > Created Time: 2023年04月27日 星期四 10时56分21秒
 ************************************************************************/

#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "CGImysql/sql_connection_pool.h"
#include "config.h"

using namespace std;

string getRandStr(int n) {
  string str = "";
  for (int i = 0; i < n; i++) {
    str += (rand() % 26) + 97;
  }
  return str;
}

string sqlCommand() {
  string sql = "insert into mydata (name, time) values (\"" + getRandStr(3) + "\", \"" + getRandStr(6) + "\")";
  return sql;
}

void doInsert(connection_pool* pool) {
  string sql = sqlCommand();
  MYSQL* mysql = NULL;
  connectionRAII conn(&mysql, pool);
  mysql_query(mysql, "use test");
  mysql_query(mysql, sql.c_str());
}
enum mm { a = 0, b, c };
int main(int argc, char* argv[]) {
  // cout << argc << endl;
  // int opt;
  // while (true) {
  //   opt = getopt(argc, argv, "a:b:c:d:efg");
  //   if (opt == -1) {
  //     break;
  //   }
  //   cout << optarg << endl;
  // }
  printf("%s\n", getRandStr(10).c_str());
  return 0;
}