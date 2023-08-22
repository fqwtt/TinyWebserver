#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <queue>
#include <thread>

#include "../CGImysql/sql_connection_pool.h"
#include "../lock/locker.h"
using namespace std;

template <typename T>
class threadpool {
 public:
  // thread_number是线程池中线程的数量，max_requests是请求队列中最多允许的、等待处理的请求的数量
  threadpool(int actor_model, connection_pool* connPool, int thread_number = 8, int max_requests = 10000);
  ~threadpool();
  bool append(T* request, int state);
  bool append_p(T* request);

 private:
  static void* worker(void* arg);
  void run();

 private:
  int m_thread_num;
  int m_max_requests;
  thread* m_threads;
  queue<T*> m_workqueue;
  locker m_queuelocker;
  sem m_queuestat;
  connection_pool* m_connPoll;
  int m_actor_model;
};

template <typename T>
threadpool<T>::threadpool(int actor_model, connection_pool* connPool, int thread_number, int max_requests)
    : m_actor_model(actor_model),
      m_connPoll(connPool),
      m_thread_num(thread_number),
      m_max_requests(max_requests),
      m_threads(NULL) {
  if (thread_number <= 0 || max_requests <= 0) {
    throw exception();
  }
  m_threads = new thread[m_thread_num];
  if (!m_threads) {
    throw exception();
  }
  for (int i = 0; i < m_thread_num; i++) {
    thread tmp(worker, this);
    m_threads[i] = std::move(tmp);
    m_threads[i].detach();
  }
}

template <typename T>
threadpool<T>::~threadpool() {
  delete[] m_threads;
}

template <typename T>
bool threadpool<T>::append(T* request, int state) {
  m_queuelocker.lock();
  if (m_workqueue.size() >= m_max_requests) {
    m_queuelocker.unlock();
    return false;
  }
  request->m_state = state;
  m_workqueue.push(request);
  m_queuelocker.unlock();
  m_queuestat.post();
  return true;
}

template <typename T>
bool threadpool<T>::append_p(T* request) {
  m_queuelocker.lock();
  if (m_workqueue.size() >= m_max_requests) {
    m_queuelocker.unlock();
    return false;
  }
  m_workqueue.push(request);
  m_queuelocker.unlock();
  m_queuestat.post();
  return true;
}

template <typename T>
void* threadpool<T>::worker(void* arg) {
  threadpool* pool = (threadpool*)arg;
  pool->run();
  return pool;
}

template <typename T>
void threadpool<T>::run() {
  while (true) {
    m_queuestat.wait();
    m_queuelocker.lock();
    if (m_workqueue.empty()) {
      m_queuelocker.unlock();
      continue;
    }
    T* request = m_workqueue.front();
    m_workqueue.pop();
    m_queuelocker.unlock();
    if (!request) continue;
    if (1 == m_actor_model) {
      if (0 == request->m_state) {
        if (request->read_once()) {
          request->improv = 1;
          connectionRAII mysqlcon(&request->mysql, m_connPoll);
          request->process();
        } else {
          request->improv = 1;
          request->timer_flag = 1;
        }
      } else {
        if (request->write()) {
          request->improv = 1;
        } else {
          request->improv = 1;
          request->timer_flag = 1;
        }
      }
    } else {
      connectionRAII mysqlcon(&request->mysql, m_connPoll);
      request->process;
    }
  }
}
#endif