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

//유제 레벨 오브젝트이다.
condition_variable cv;
//#include <condition_variable>
//condition_variable_any cva;

void Producer()
{
	while (true)
	{
		// 락잡고
		// 공유변수 수정
		// 락 풀고
		// 조건변수로 다른 스레드에게 통지
		// 
		//
		{
			unique_lock<mutex> lock(m);
			q.push(100);
		}
		//::SetEvent(handle); //시그널 상태로 변환
		//this_thread::sleep_for(100ms);
		cv.notify_one(); //wait중인 딱 한 개의 스레드 깨운다
	}
}
void Consumer()
{
	while (true)
	{
		//::WaitForSingleObject(handle, INFINITE);
		//이 부분 오자마자 Producer한테 바로 넘어갈 수 있음
	
		unique_lock<mutex> lock(m);
		cv.wait(lock, []() { return q.empty() == false; }); //큐 비어있지 않을 때 까지 기다림
		// 락잡고
		// 조건확인
		//	만족하면 탈출 후 진행
		//	만족 안 하면,락 풀고 대기
	
		//Spurious Wakeup(가짜 기상)
		//notify_one은 lock 잡혀있는 게 아니기 때문에, 조건이 필요함
		{
			cout << q.front() << endl;
			q.pop();
		}
	}
}


int main()
{

	handle = CreateEvent(NULL, FALSE/*자동 이벤트*/, FALSE, NULL);
	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(handle);
}
