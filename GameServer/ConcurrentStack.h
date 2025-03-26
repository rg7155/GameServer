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

template <class T>
class LockFreeStack
{
public:
	struct Node
	{
		Node(const T& value) : data(value), next(nullptr) {};
		T data;
		Node* next;
	};

	//[head] [head->next] []...
	void Push(const T& value)
	{
		Node* node = new Node(value);
		node->next = _head;

		//_head = node;
		while (_head.compare_exchange_strong(node->next, node) == false)
		{
			node->next = _head;
		}
	}

	/*
	* head 읽기
	* head->next
	* head = head->next
	* data 추출
	* data 삭제
	*/
	bool TryPop(T& value)
	{
		++_popCount;
		Node* oldHead = _head;
		while (oldHead && _head.compare_exchange_strong(oldHead, oldHead->next) == false)
		{
		}

		if (oldHead == nullptr)
		{
			--_popCount;
			return false;
		}

		value = oldHead->data;

		//delete oldHead;
		TryDelete(oldHead);

		return true;
	}

	void TryDelete(Node* oldHead)
	{
		if (_popCount == 1)
		{
			//나 혼자임
			Node* node = _pendingList.exchange(nullptr); //exchange, read, write 동시에
			
			if (--_popCount == 0)
			{
				//분리하고 다시 끼어들지 않았으면
				DeleteNodes(node);
			}
			else if(node)
			{
				//_pendingList 꺼낸 이후에 누가 또 끼어들었으면 다시 갖다 놓음
				ChainPendingNodeList(node);
			}
			delete oldHead;
		}
		else
		{
			//혼자 아님, 삭제 예약
			ChainPendingNode(oldHead);
		}
	}

	void ChainPendingNodeList(Node* first, Node* last)
	{
		//인자의 노드와 pendinglist를 연결하고, pendinglist로 설정함
		//[first][][last] -> [pendinglist]
		last->next = _pendingList;
		while (_pendingList.compare_exchange_strong(last->next, first) == false)
		{
		}
	}
	void ChainPendingNodeList(Node* first)
	{
		//헬퍼
		Node* last = first;
		while (last)
		{
			last = last->next;
		}
		ChainPendingNodeList(first, last);
	}
	void ChainPendingNode(Node* node)
	{
		ChainPendingNodeList(node, node);
	}

	static void DeleteNodes(Node* node)
	{
		//노드 리스트를 모두 삭제
		while (node)
		{
			Node* next = node->next;
			delete node;
			node = next;
		}
	}

private:
	atomic<Node*> _head;
	atomic<uint32> _popCount = 0; //pop 실행중인 스레드 갯수
	atomic<Node*> _pendingList; //삭제되야 할 노드들 (첫 노드 주소)

};
