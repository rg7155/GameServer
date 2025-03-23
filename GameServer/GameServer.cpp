#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>


class SpinLock
{
public:
	//lock_guard와 인터페이스 같아야 하므로 소문자 lock
	void lock()
	{
		//while (_locked)
		//{

		//}
		//_locked = true;

		//CAS로 해결
		bool expected = false;
		bool desired = true;

		//의사 코드
		//if (_locked == expected)
		//{
		//	expected = _locked;
		//	_locked = desired;
		//	return true;
		//}
		//else
		//{
		//	expected = _locked;
		//	return false;
		//}

		//실패하면 계속 시도해라
		while (_locked.compare_exchange_strong(expected, desired) == false)
		{
			//함수 인자를 레퍼런스로 받아서 값이 바뀌므로 다시 초기값으로 세팅
			expected = false;
		}


			
	}
	void unlock()
	{
		//_locked = false;
		_locked.store(false);//atomic 함수를 사용하여 변경
	}
private:
	//volatile bool _locked = false;//단순 컴파일러 최적화 하지 마라
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
