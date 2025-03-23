#include "pch.h"
#include <iostream>
#include "CorePch.h"

//표준 라이브러리 사용
//윈도우즈,리눅스 환경 등 호환
#include <thread>
#include <atomic>

atomic<int32> sum = 0;
void Add()
{
	for (int32 i = 0; i < 1000000; ++i)
	{
		//디버그->창->디스어셈블리
		//cpu에서 레지스터에 읽고, 연산하고, 쓰는 3가지 과정
		//sum++;

		sum.fetch_add(1); //atomic 인자임을 확인 가능
	}
}

void Sub()
{
	for (int32 i = 0; i < 1000000; ++i)
	{
		sum.fetch_add(-1);
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
