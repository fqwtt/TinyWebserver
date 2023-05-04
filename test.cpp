/*************************************************************************
  > File Name: test.cpp
  > Author: fqwtt
  > Created Time: 2023年04月27日 星期四 22时08分32秒
 ************************************************************************/

#include <iostream>
#include <mysql/mysql.h>
using namespace std;
int main(){
	MYSQL mysql;
	string host="127.0.0.1";
	string user="fqwtt";
	string passwd="123.com";
	string database="test";
	if (mysql_init(&mysql)==NULL){
		cout<<"init err"<<endl;
	}
	if(mysql_real_connect(&mysql,host.c_str(),user.c_str(),passwd.c_str(),database.c_str(),0,NULL,0)==NULL){
		cout<<"conn err"<<endl;
	}
	string command = "select * from tabel1;";
	mysql_query(&mysql, command.c_str());
	MYSQL_RES *result;
	result = mysql_store_result( &mysql );
	int num;
	num = mysql_num_fields( result );  //返回字段个数
	for( int i = 0; i < num; i++ ) {
		MYSQL_FIELD* field = mysql_fetch_field_direct( result, i );  //返回字段类型
		cout << field->name << "\t\t";  //输出字段名
	}
	cout << endl;
	MYSQL_ROW row;
	while( row = mysql_fetch_row( result ), row ) {
		for( int i = 0; i < num; i++ ) {
			cout << row[i] << "\t\t";
		}
		cout << endl;
	}
	mysql_close(&mysql);
	return 0;
}
