#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>


atomic<bool> flag;

atomic<bool> ready;
int32 value;
void Producer()
{
	value = 10;
	ready.store(true, memory_order::memory_order_release);
	//atomic_thread_fence(memory_order::memory_order_release);
	//=====================
}
void Consumer()
{
	//=====================
	while (ready.load(memory_order::memory_order_acquire) == false)
		;
	cout << value;
}

int main()
{
	flag = false;
	//flag.is_lock_free();
	flag.store(true, memory_order::memory_order_seq_cst);
	bool res = flag.load();

	//이전 flag 값을 prev에 넣고 flag 값 수정할 때
	{
		//bool prev = flag;
		//flag = true;

		bool prev = flag.exchange(true);
	}

	{
		/*
		* 메모리 모델 정책
		* 1.seq_cst (컴파일러 최적화 적음, )
		* 2.acquire, release
		* 3.relaxed
		*/
	}
}
