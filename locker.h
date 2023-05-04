/*************************************************************************
  > File Name: locker.h
  > Author: fqwtt
  > Created Time: 2023年05月04日 星期四 11时20分12秒
 ************************************************************************/
#ifndef LOCKER_H
#define LOCKER_H
#include <pthread.h>
#include <semaphore.h>
#include <exception>
#include <time.h>
class sem{
	public:
		sem(){
			if(sem_init(&this->m_sem,0,0)!=0){
				throw std::exception();
			}
		}

		sem(int num){
			if(sem_init(&this->m_sem,0,num)!=0){
				throw std::exception();
			}
		}

		~sem(){
			sem_destroy(&this->m_sem);
		}

		bool wait(){
			return sem_wait(&this->m_sem);
		}

		bool post(){
			return sem_post(&this->m_sem);
		}

	private:
		sem_t m_sem;
};

class locker{
	public:
		locker(){
			if(pthread_mutex_init(&this->m_mutex,NULL)!=0){
				throw std::exception();
			}
		}

		~locker(){
			pthread_mutex_destroy(&this->m_mutex);
		}

		bool lock(){
			return pthread_mutex_lock(&this->m_mutex)==0;
		}

		bool unlock(){
			return pthread_mutex_unlock(&this->m_mutex)==0;
		}

		pthread_mutex_t* get(){
			return &this->m_mutex;
		}

	private:
		pthread_mutex_t m_mutex;
};

class cond{
	public:
		cond(){
			if(pthread_cond_init(&this->m_cond,NULL)!=0){
				throw std::exception();
			}
		}

		~cond(){
			pthread_cond_destroy(&this->m_cond);
		}

		bool wait(pthread_mutex_t* m_mutex){
			int ret=0;
			ret=pthread_cond_wait(&this->m_cond,m_mutex);
			return ret==0;
		}

		bool timewait(pthread_mutex_t* m_mutex,struct timespec t){
			int ret=0;
			ret=pthread_cond_timedwait(&this->m_cond,m_mutex,&t);
			return ret==0;
		}

		bool signal(){
			return pthread_cond_signal(&this->m_cond)==0;
		}

		bool broadcast(){
			return pthread_cond_broadcast(&this->m_cond)==0;
		}

	private:
		pthread_cond_t m_cond;
};
#endif
