#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>


int64 Calculate()
{
	int64 sum = 0;
	for(int32 i = 0; i < 100000; ++i)
		sum += i;
	return sum;
}

void PromiseWorker(promise<string>&& promise)
{
	promise.set_value("MSG");
}

int main()
{
	{
		/*
		* 1.deferred->lazy evaluation, 지연실행, 그냥 나중에 호출하는거
		* 2.async->별도 스레드 생성 후 실행
		* 3.deferred | async->
		*/
		future<int64> future = async(launch::async, Calculate);

		//1ms만 기다려보겠다, 상태 잠시 확인
		//future_status status = future.wait_for(1ms);
		//if (status == future_status::ready)
		//{

		//}

		//계속 기다림, get 하는 거나 똑같음
		future.wait();

		int64 sum = future.get();


		//객체멤버 함수
		//Knight knight;
		//future<int64> future2 = async(launch::async, &Knight::GetHp, knight); //knight.GetHp()

	}

	{
		//미래에 결과물 반환해줄거라는 약속?
		promise<string> promise;
		future<string> future = promise.get_future();

		//future는 메인스레드가 갖고, promise는 워커스레드가 갖는다
		thread t(PromiseWorker, move(promise));
		string msg = future.get();
		cout << msg << endl;
		t.join();
	}
}
