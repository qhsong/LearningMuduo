/*
 * =====================================================================================
 *
 *       Filename:  CopyOnWrite_test.cc
 *
 *    Description:  A standar copy on write test
 *
 *        Version:  1.0
 *        Created:  2014年09月14日 19时20分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  qhsong (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include<muduo/base/Mutex.h>
#include<muduo/base/Thread.h>
#include<vector>
#include<boost/shared_ptr.hpp>
#include<stdio.h>

using namespace muduo;

class Foo {
	public:
		void doit() const;
};

typedef std::vector<Foo> Foolist;
typedef boost::shared_ptr<Foolist> FooListPtr;

FooListPtr g_foos;
MutexLock mutex;

void post(const Foo& f) {
	printf("post\n");
	MutexLockGuard lock(mutex);
	if(!g_foos.unique()) {
		g_foos.reset(new FooList(*g_foos));
		printf("Copy the hole list\n");
	}
	assert(g_foos.unique());
	g_foos->push_back(f);
}

void traverse() {
	FooListPtr foos;
	{
		MutexLockGuard lock(mutex);
		foos = g_foos;
		assert(!g_foos.unique());
	}

	assert(!foos.unique());
	for(std::vector<Foo>::const_iterator it = foos->begin();
		it!= foos->end();++it) {
		it ->doit();
	}
}

void Foo::doit() const {
	Foo f;
	post(f);
}

int main() {
	g_foos.reset(new FooList);
	Foo f;
	post(f);
	traverse();
}
