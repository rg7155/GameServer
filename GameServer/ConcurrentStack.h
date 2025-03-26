#pragma once
#include <mutex>
template <class T>
class LockStack
{
public:
	LockStack() {};

	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_stack.push(value);
		_condVar.notify_one();
	}
	
	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty())
			return false;

		value = move(value);
		_stack.pop();
		return true;
	}

	void WaitPop(T& value)
	{
		unique_lock <mutex> lock(_mutex);
		_condVar.wait(lock, [this]() { return _stack.empty() == false; });
		value = move(value);
		_stack.pop();
	}

	stack<T> _stack;
	mutex _mutex;
	condition_variable _condVar;
};

//template <class T>
//class LockFreeStack
//{
//public:
//	struct Node
//	{
//		Node(const T& value) : data(make_shared<T>(value)), next(nullptr) {};
//		shared_ptr<T> data;
//		shared_ptr<Node> next;
//	};
//
//	void Push(const T& value)
//	{
//		shared_ptr<Node> node = make_shared<Node>(value);
//		node->next = atomic_load(&_head);
//
//		//while (_head.compare_exchange_strong(node->next, node) == false)
//		while (atomic_compare_exchange_weak(&_head, &node->next, node) == false)
//
//		{
//			node->next = _head;
//		}
//	}
//
//	shared_ptr<T> TryPop()
//	{
//		shared_ptr<Node> oldHead = atomic_load(&_head);//ref 증가,꺼내오는거 한번에
//		while (oldHead && atomic_compare_exchange_weak(&_head, &oldHead, oldHead->next) == false)
//		{
//
//		}
//		if (!oldHead)
//			return shared_ptr<T>();
//
//		return oldHead->data;
//	}
//
//
//
//private:
//	shared_ptr<Node> _head;
//
//
//};

template<typename T>
class LockFreeStack
{
	struct Node;

	struct CountedNodePtr
	{
		int32 externalCount = 0;
		Node* ptr = nullptr;
	};

	struct Node
	{
		Node(const T& value) : data(make_shared<T>(value))
		{

		}

		shared_ptr<T> data;
		atomic<int32> internalCount = 0;
		CountedNodePtr next;
	};

public:
	void Push(const T& value)
	{
		CountedNodePtr node;
		node.ptr = new Node(value);
		node.externalCount = 1;
		node.ptr->next = _head;
		while (_head.compare_exchange_weak(node.ptr->next, node) == false)
		{
		}
	}

	shared_ptr<T> TryPop()
	{
		CountedNodePtr oldHead = _head;
		while (true)
		{
			// 참조권 획득
			// 헤드를 참조하기 위해, externalCount를 사용
			IncreaseHeadCount(oldHead);
			Node* ptr = oldHead.ptr;

			// 데이터 없음
			if (ptr == nullptr)
				return shared_ptr<T>();

			// 소유권 획득 (ptr->next로 head를 바꿔치기 한 애가 이김)
			if (_head.compare_exchange_strong(oldHead, ptr->next))
			{
				shared_ptr<T> res;
				res.swap(ptr->data);

				// external : 1 -> 2(나+1) -> 4(나+1 남+2)
				// internal : 1 -> 0
				const int32 countIncrease = oldHead.externalCount - 2;

				//fetch_add는 더하기 이전 값을 리턴한다
				//만약 countIncrease가 0일 때 성공,
				if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
					delete ptr;

				return res;
			}
			else if (ptr->internalCount.fetch_sub(1) == 1)
			{
				// 참조권은 얻었으나, 소유권은 실패
				// 제일 마지막에 나오는 얘가 삭제하고 나온다.
				delete ptr;
			}
		}
	}

private:
	void IncreaseHeadCount(CountedNodePtr& oldCounter)
	{
		while (true)
		{
			CountedNodePtr newCounter = oldCounter;
			newCounter.externalCount++;

			if (_head.compare_exchange_strong(oldCounter, newCounter))
			{
				oldCounter.externalCount = newCounter.externalCount;
				break;
			}
		}
	}

private:
	atomic<CountedNodePtr> _head;
};