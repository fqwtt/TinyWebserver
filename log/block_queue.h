/*************************************************************************
  > File Name: block_queue.h
  > Author: fqwtt
  > Created Time: 2023年05月04日 星期四 14时20分19秒
 ************************************************************************/
#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H
#include <sys/time.h>

#include <iostream>
#include <queue>

#include "../locker/locker.h"
using namespace std;

template <typename T>
class block_queue {
 public:
  block_queue(int max_size = 1000) {
    if (max_size <= 0) exit(-1);
    this->m_max_size = max_size;
  }

  void clear() {
    this->m_mutex.lock();
    queue<T> empty_queue;
    this->m_queue.swap(empty_queue);
    this->m_size = 0;
    this->m_front = -1;
    this->m_back = -1;
    this->m_mutex.unlock();
  }

  bool full() {
    bool flag = false;
    this->m_mutex.lock();
    if (this->m_queue.size() >= this->m_max_size) flag = true;
    this->m_mutex.unlock();
    return flag;
  }

  bool empty() {
    this->m_mutex.lock();
    return this->m_queue.empty();
    this->m_mutex.unlock();
  }

  bool front(T& val) {
    bool flag = false;
    this->m_mutex.lock();
    if (this->m_queue.size() != 0) {
      flag = true;
      val = this->m_queue.front();
    }
    this->m_mutex.unlock();
    return flag;
  }

  bool back(T& val) {
    bool flag = false;
    this->m_mutex.lock();
    if (this->m_queue.size() != 0) {
      flag = true;
      val = this->m_queue.back();
    }
    this->m_mutex.unlock();
    return flag;
  }

  int size() {
    this->m_mutex.lock();
    int tmp = this->m_queue.size();
    this->m_mutex.unlock();
    return tmp;
  }

  int max_size() {
    this->m_mutex.lock();
    int tmp = this->m_max_size;
    this->m_mutex.unlock();
    return tmp;
  }

  bool push(const T& item) {
    this->m_mutex.lock();
    if (this->m_queue.size() >= this->m_max_size) {
      this->m_cond.broadcast();
      this->m_mutex.unlock();
      return false;
    }
    this->m_queue.push(item);
    this->m_cond.broadcast();
    this->m_mutex.unlock();
    return true;
  }

  bool pop(T& item) {
    this->m_mutex.lock();
    while (this->m_queue.size() <= 0) {
      if (!this->m_cond.wait(this->m_mutex.get())) {
        this->m_mutex.unlock();
        return false;
      }
    }
    item = this->m_queue.front();
    this->m_queue.pop();
    this->m_mutex.unlock();
    return true;
  }

  bool pop(T& item, int ms_timeout) {
    struct timespec t = {0, 0};
    struct timeval now = {0, 0};
    gettimeofday(&now, NULL);
    this->m_mutex.lock();
    if (this->m_queue.size() <= 0) {
      t.tv_sec = now.tv_sec + ms_timeout / 1000;
      t.tv_nsec = (ms_timeout % 1000) * 1000;
      if (!this->m_cond.timewait(this->m_mutex.get(), t)) {
        this->m_mutex.unlock();
        return false;
      }
    }

    if (this->m_queue.size() <= 0) {
      this->m_mutex.unlock();
      return false;
    }

    item = this->m_queue.front();
    this->m_queue.pop();
    this->m_mutex.unlock();
    return true;
  }

 private:
  locker m_mutex;
  cond m_cond;

  queue<T> m_queue;
  int m_max_size;
};

#endif
