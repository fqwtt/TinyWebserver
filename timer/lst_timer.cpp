/*************************************************************************
  > File Name: lst_timer.cpp
  > Author: fqwtt
  > Created Time: 2023年05月31日 星期三 9时11分25秒
 ************************************************************************/

#include "lst_timer.h"

using namespace std;

sort_timer_lst::sort_timer_lst() {
  this->head = nullptr;
  this->tail = nullptr;
}

sort_timer_lst::~sort_timer_lst() {
  util_timer* tmp = head;
  while (tmp) {
    head = tmp->next;
    delete tmp;
    tmp = head;
  }
}

void sort_timer_lst::add_timer(util_timer* timer) {}

void sort_timer_lst::adjust_timer(util_timer* timer) {}

void sort_timer_lst::del_timer(util_timer* timer) {}

void sort_timer_lst::tick() {}
