#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>

thread_local int32 LThreadID = 0;
//int32 LThreadID = 0;

void th(int32 id)
{
	LThreadID = id;
	while (true)
	{
		cout << LThreadID << endl;
		this_thread::sleep_for(1s);
	}
}

int main()
{
	vector<thread> threads;
	for (int32 i = 0; i < 10; ++i)
	{
		threads.push_back(thread(th, i + 1));
	}

	for (int32 i = 0; i < 10; ++i)
		threads[i].join();

}
