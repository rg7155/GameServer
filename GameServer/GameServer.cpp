#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ConcurrentStack.h"
#include "ConcurrentQueue.h"

LockQueue<int32> q;
//LockStack<int32> s;
LockFreeStack<int32> s;


void Push()
{
	while (true)
	{
		int32 value = rand() % 100;
		//q.Push(value);
		s.Push(value);

		this_thread::sleep_for(10ms);
	}
}

void Pop()
{
	while (true)
	{
		auto data = s.TryPop();
		if (data)
			cout << (*data) << endl;
	}
}


int main()
{
	thread t1(Push);
	thread t2(Pop);
	thread t3(Pop);

	t1.join();
	t2.join();
	t3.join();
}