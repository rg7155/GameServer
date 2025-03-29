#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

#include <vector>
#include <thread>

atomic<int> primeCount = 0;
bool isPrime(int number)
{
	if (number <= 1)
		return false;
	if (number <= 3)
		return true;

	for (int i = 2; i * i <= number; ++i)
	{
		if ((number % i) == 0)
			return false;
	}
	return true;
}
void countPrime(int start, int end)
{
	for (int i = start; i <= end; ++i)
	{
		if (isPrime(i))
			primeCount.fetch_add(1);
	}
}

int main()
{
	const int MAX_NUM = 100'0000;
	int coreCount = thread::hardware_concurrency();
	int jobCount = MAX_NUM / coreCount;
	vector<thread> threads;
	for (int i = 0; i < coreCount; ++i)
	{
		int start = i * jobCount;
		int end = (i + 1) * jobCount;

		thread t = thread(countPrime, start, end);
		threads.push_back(move(t)); //thread 복사생성자 금지

		//threads.push_back(thread(countPrime, start, end));
	}
	for (int i = 0; i < threads.size(); ++i)
	{
		threads[i].join();
	}

	cout << primeCount;
}