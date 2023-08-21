/*************************************************************************
  > File Name: log.cpp
  > Author: fqwtt
  > Created Time: 2023年05月04日 星期四 17时11分41秒
 ************************************************************************/

#include "./log.h"

#include <stdarg.h>

#include <cstring>
#include <iostream>
using namespace std;

Log::Log() {
  this->m_count = 0;
  this->m_is_async = false;
}

Log::~Log() {
  if (this->m_fp != nullptr) {
    fclose(this->m_fp);
  }
}

bool Log::init(const char* file_name, int close_log, int log_buf_size, int max_lines, int max_queue_size) {
  if (max_queue_size >= 1) {
    this->m_is_async = true;
    this->m_log_queue = new block_queue<string>(max_queue_size);
    pthread_t tid;
    pthread_create(&tid, nullptr, flush_log_thread, nullptr);
  }
  this->m_close_log = close_log;
  this->m_log_buf_size = log_buf_size;
  this->m_buf = new char[log_buf_size];
  memset(this->m_buf, '\0', log_buf_size);
  this->m_max_lines = max_lines;

  time_t t = time(nullptr);
  struct tm* sys_tm = localtime(&t);
  struct tm my_tm = *sys_tm;

  const char* p = strrchr(file_name, '/');
  char log_full_name[256] = {0};

  if (p == nullptr) {
    snprintf(log_full_name, 255, "%d_%02d_%02d_%s", my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday, file_name);
  } else {
    strcpy(this->log_name, p + 1);
    strncpy(this->dir_name, file_name, p - file_name + 1);
    snprintf(log_full_name, 255, "%s%d_%02d_%02d_%s", this->dir_name, my_tm.tm_year + 1900, my_tm.tm_mon + 1,
             my_tm.tm_mday, this->log_name);
  }
  this->m_today = my_tm.tm_mday;
  this->m_fp = fopen(log_full_name, "a");
  if (this->m_fp == nullptr) {
    return false;
  }
  return true;
}

void Log::write_log(int level, const char* format, ...) {
  struct timeval now = {0, 0};
  gettimeofday(&now, nullptr);
  time_t t = now.tv_sec;
  struct tm* sys_tm = localtime(&t);
  struct tm my_tm = *sys_tm;
  char s[16] = {0};
  switch (level) {
    case 0:
      strcpy(s, "[debug]:");
      break;
    case 1:
      strcpy(s, "[info]:");
      break;
    case 2:
      strcpy(s, "[warn]:");
      break;
    case 3:
      strcpy(s, "[erro]:");
      break;
    default:
      strcpy(s, "[info]:");
      break;
  }

  m_mutex.lock();
  m_count++;

  if (this->m_today != my_tm.tm_mday || this - m_count % this->m_max_lines == 0) {
    char new_log[256] = {0};
    fflush(this->m_fp);
    fclose(this->m_fp);
    char tail[16] = {0};

    snprintf(tail, 16, "%d_%02d_%02d_", my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday);
    if (this->m_today != my_tm.tm_mday) {
      snprintf(new_log, 255, "%s%s%s", dir_name, tail, log_name);
      this->m_today = my_tm.tm_mday;
      this->m_count = 0;
    } else {
      snprintf(new_log, 255, "%s%s%s.%lld", dir_name, tail, log_name, this->m_count / this->m_max_lines);
    }
    this->m_fp = fopen(new_log, "a");
  }

  this->m_mutex.unlock();

  va_list valst;
  va_start(valst, format);
  string log_str;
  this->m_mutex.lock();

  int n = snprintf(m_buf, 48, "%d-%02d-%02d %02d:%02d:%02d.%06ld %s ", my_tm.tm_year + 1900, my_tm.tm_mon + 1,
                   my_tm.tm_mday, my_tm.tm_hour, my_tm.tm_min, my_tm.tm_sec, now.tv_usec, s);
  int m = vsnprintf(this->m_buf + n, this->m_log_buf_size - n - 1, format, valst);
  this->m_buf[n + m] = '\n';
  this->m_buf[n + m + 1] = '\0';
  log_str = this->m_buf;

  this->m_mutex.unlock();

  if (this->m_is_async && !this->m_log_queue->full()) {
    this->m_log_queue->push(log_str);
  } else {
    this->m_mutex.lock();
    fputs(log_str.c_str(), this->m_fp);
    this->m_mutex.unlock();
  }

  va_end(valst);
}

void Log::flush() {
  this->m_mutex.lock();
  fflush(this->m_fp);
  this->m_mutex.unlock();
}
