#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>


mutex m;
queue<int32> q;
HANDLE handle;

void Producer()
{
	while (true)
	{
		{
			unique_lock<mutex> lock(m);
			q.push(100);
		}
		::SetEvent(handle); //시그널 상태로 변환
		this_thread::sleep_for(100ms);
	}
}
void Consumer()
{
	//Producer의 sleep이 길어도 계속 확인을 해야하는 비효율적 상황
	while (true)
	{
		::WaitForSingleObject(handle, INFINITE);
		//수동 이벤트이므로 다시 NonSignal
		//만약 자동 이벤트라면
		//::ResetEvent(handle);
		unique_lock<mutex> lock(m);
		if(!q.empty())
		{
			cout << q.front() << endl;
			q.pop();
		}
	}
}


int main()
{
	//커널 오브젝트임, 반환값은 번호표 같은 것
	// Usage Count
	// Signal(파란불), Non Signal (빨간불)
	// Auto(자동), Manual(수동)
	//
	handle = CreateEvent(NULL, FALSE/*자동 이벤트*/, FALSE, NULL);
	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(handle);
}
