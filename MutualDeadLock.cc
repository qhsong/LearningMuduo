/*
 * =====================================================================================
 *
 *       Filename:  MutualDeadLock.cc
 *
 *    Description:  A mutual Dead Lock example
 *
 *        Version:  1.0
 *        Created:  2014年09月14日 15时35分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  qhsong (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include<muduo/base/Mutex.h>
#include<muduo/base/Thread.h>
#include<set>
#include<stdio.h>

class Request;

class Inventory {
	public:
		void add(Request *req) {
			muduo::MutexLockGuard lock(mutex_);
			requests_.insert(req);
		}

		void remove(Request* req) {
			muduo::MutexLockGuard lock(mutex_);
			requests_.erase(req);
		}

		void printAll() const;

	private:
		mutable muduo::MutexLock mutex_;
		std::set<Request*> requests_;
};

Inventory g_inventory;

class Request {
	public:
		void process() {
			muduo::MutexLockGuard lock(mutex_);
			g_inventory.add(this);
		}

		~Request() {
			muduo::MutexLockGuard lock(mutex_);
			sleep(1);
			g_inventory.remove(this);
		}

		void print() const __attribute__((noinline)) {
			muduo::MutexLockGuard lock(mutex_);
		}
	private:
		mutable muduo::MutexLock mutex_;
};

void Inventory::printAll() const {
	muduo::MutexLockGuard lock(mutex_);
	sleep(1);
	for(std::set<Request*>::const_iterator it = requests_.begin();
			it != requests_.end();++it){
		(*it)->print();
	}

	printf("Inventory::printAll unlocked\n");
}

void threadFunc() {
	Request *req = new Request;
	req->process();
	delete req;
}

int main() {
	muduo::Thread thread(threadFunc);
	thread.start();
	usleep(500*1000);
	g_inventory.printAll();
	thread.join();
}
