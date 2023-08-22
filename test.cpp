#include <cstring>
#include <iostream>
#include <queue>
#include <thread>

#include "./lock/locker.h"
#include "./log/log.h"

using namespace std;
class threadPool {
 public:
  threadPool(int thread_number, int max_queue_size) {
    m_thread_number = thread_number;
    m_max_queue_size = max_queue_size;
    vector<thread> m_threads;
    for (int i = 0; i < thread_number; i++) {
      m_threads.emplace_back(thread(worker, this));
      m_threads[i].detach();
    }
  }
  void run() {
    while (true) {
      m_sem.wait();
      m_locker.lock();
      if (works.empty()) {
        m_locker.unlock();
        continue;
      }
      int n = works.front();
      works.pop();
      cout << std::this_thread::get_id() << ":" << n << endl;
      m_locker.unlock();
    }
  }
  static void* worker(void* args) {
    threadPool* pool = (threadPool*)args;
    pool->run();
    return pool;
  }

  void append(int num) {
    m_locker.lock();
    works.push(num);
    m_locker.unlock();
    m_sem.post();
  }

 public:
  sem m_sem;
  locker m_locker;
  int m_thread_number;
  int m_max_queue_size;
  queue<int> works;
};

int main() {
  threadPool threads(2, 4);
  while (true) {
    int a;
    cin >> a;
    threads.append(a);
  }
  return 0;
}