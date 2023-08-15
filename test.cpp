/*************************************************************************
  > File Name: test.cpp
  > Author: fqwtt
  > Created Time: 2023年04月27日 星期四 22时08分32秒
 ************************************************************************/

#include <fcntl.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <iostream>

int main() {
  // 创建文件描述符
  int fd = open("../test.txt", O_RDONLY);
  if (fd < 0) {
    std::cerr << "Failed to open file." << std::endl;
    return -1;
  }

  // 将文件描述符设置为非阻塞模式
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);

  // 读取文件
  char buffer[1024];
  int n = read(fd, buffer, sizeof(buffer));
  if (n < 0) {
    std::cerr << "Failed to read file." << std::endl;
  } else {
    std::cout << "Read " << n << " bytes: " << buffer << std::endl;
  }

  // 恢复文件描述符的阻塞模式
  fcntl(fd, F_SETFL, old_option);

  // 关闭文件描述符
  close(fd);

  return 0;
}
