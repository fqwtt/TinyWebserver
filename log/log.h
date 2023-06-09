/*************************************************************************
  > File Name: log.h
  > Author: fqwtt
  > Created Time: 2023年05月04日 星期四 15时38分18秒
 ************************************************************************/
#ifndef LOG_H
#define LOG_H

#include <iostream>
#include "./block_queue.h"
#include "../locker/locker.h"
using namespace std;

class Log{
	public:
		static Log* get_instance(){
			static Log instance;
			return &instance;
		}
		static void *flush_log_thread(void* args){
			Log::get_instance()->async_write_log();
		}

		bool init(const char* file_name,int close_log,int log_buf,int log_buf_size=8192,int max_lines=5000000,int max_queue_size=0);

		void write_log(int level,const char* format,...);

		void flush();

	private:
		Log();
		virtual ~Log();
		void *async_write_log(){
			string single_log;
			while(this->m_log_queue->pop(single_log)){
				this->m_mutex.lock();
				fputs(single_log.c_str(),m_fp);
				this->m_mutex.unlock();
			}
		}
	private:
		char dir_name[128];
		char log_name[128];
		int m_max_lines;
		int m_log_buf_size;
		long long m_count;
		int m_today;
		FILE* m_fp;
		char* m_buf;
		block_queue<string>* m_log_queue;
		bool m_is_async;
		locker m_mutex;
		int m_close_log;
};
#endif
