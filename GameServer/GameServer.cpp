#include "pch.h"
#include <iostream>
#include "CorePch.h"

//표준 라이브러리 사용
//윈도우즈,리눅스 환경 등 호환
#include <thread>

void HelloThread()
{
	cout << "Hello CGH" << endl;
}

int main()
{
	//시스템 콜, 커널모드 요청이라 무겁다
	//cout << "Hello CGH" << endl;
	HelloThread();

	//독립적은 스레드 생성되며, 병렬 처리 된다.
	thread t(HelloThread);

	int32 count = t.hardware_concurrency(); //cpu 코어 개수, 100프로 정확은 아님
	t.get_id(); //스레드 id
	t.detach(); //실제 스레드와 분리한다. 이후엔 상태 확인 어려움. 딱히 안쓴다.
	t.joinable(); //
		

	//메인스레드가 t의 종료를 기다린다.
	t.join();
}
