#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>

vector<int32> vec;
mutex m;

template<typename T>
class LockGuard
{
public:
	LockGuard(T& m)
	{
		_mutex = &m;
		_mutex->lock();
	}
	~LockGuard()
	{
		_mutex->unlock();
	}
private:
	T* _mutex;
};

void Push()
{
	//stl은 멀티스레드 지원 안함
	//1.벡터 재할당문제, 백테 삽입 위치 문제
	for (int32 i = 0; i < 10000; ++i)
	{
		//1.재귀 락
		//2.언락 실수 
		//m.lock();
		//m.lock(); //재귀적 잠금은 불가능, recLock 따로 있음

		//LockGuard<mutex> lg(m);
		lock_guard<mutex> lg(m);
		vec.push_back(i);
		//m.unlock();
	}
}

int main()
{
	thread t1(Push);
	thread t2(Push);
	t1.join();
	t2.join();
	cout << vec.size() << endl;

}
