cmake_minimum_required(VERSION 3.10)

project(my_program)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

execute_process(COMMAND mysql_config --cflags
OUTPUT_VARIABLE MYSQL_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND mysql_config --libs
OUTPUT_VARIABLE MYSQL_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

# 添加可执行文件
add_executable(main main.cpp CGImysql/sql_connection_pool.cpp http/http_conn.cpp)
add_executable(test test.cpp)

target_compile_options(main PUBLIC ${MYSQL_CFLAGS})
target_link_libraries(main PUBLIC ${MYSQL_LIBS})