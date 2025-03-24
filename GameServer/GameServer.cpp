#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>

int32 x;
int32 y;
int32 r1;
int32 r2;
volatile bool ready;
void Thread1()
{
	while (!ready){}
		

	y = 1;
	r1 = x;
}
void Thread2()
{
	while (!ready){}

	x = 1;
	r2 = y;
}

int main()
{
	int32 count = 0;
	while (true)
	{
		ready = false;
		++count;

		x = y = r1 = r2 = 0;

		thread t1(Thread1);
		thread t2(Thread1);

		ready = true;

		t1.join();
		t2.join();

		if (r1 == 0 && r2 == 0)
			break;
	}
	cout << count << endl;
}
