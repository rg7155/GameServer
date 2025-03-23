#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>


class SpinLock
{
public:
	void lock()
	{
		bool expected = false;
		bool desired = true;

		//실패하면 계속 시도해라
		while (_locked.compare_exchange_strong(expected, desired) == false)
		{
			expected = false;

			//이때 동안 대기상태로, 스케줄링 제외
			//실패 했으면 커널로 돌아간다.
			
			//this_thread::sleep_for(chrono::microseconds(100));
			this_thread::sleep_for(0ms);//operator ms 로 쓸 수 있음
			//this_thread::yield();//sleep 0ms 와 같음
		}
	}
	void unlock()
	{
		_locked.store(false);//atomic 함수를 사용하여 변경
	}
private:
	atomic<bool> _locked = false; //volatile 기능을 하고 있다.
};

int32 sum = 0;
mutex m;
SpinLock spingLock;
void Add()
{
	for (int32 i = 0; i < 1000000; ++i)
	{
		//lock_guard<mutex> guard(m);
		lock_guard<SpinLock> guard(spingLock);
		sum++;
	}
}
void Sub()
{
	for (int32 i = 0; i < 1000000; ++i)
	{
		//lock_guard<mutex> guard(m);
		lock_guard<SpinLock> guard(spingLock);
		sum--;
	}
}

int main()
{
	thread t1(Add);
	thread t2(Sub);

	t1.join();
	t2.join();

	cout << sum << endl;
}
