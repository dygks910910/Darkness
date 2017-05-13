#pragma once

class SyncObject
{
	CRITICAL_SECTION cs;

public:
	void Enter();
	void Leave();

public:
	SyncObject();
	virtual ~SyncObject();
};


// 스코프를 이용한 lock 객체
// 스코프 내에서 생성하고 스코프를 빠져나감과 동시에 객체가 소멸되면서
// 자동으로 unlock 할 수 있도록 구현
class Lock
{
	SyncObject* sync_obj;

public:
	Lock(SyncObject* sync) : sync_obj(sync)
	{
		sync_obj->Enter();
	}

	~Lock() { sync_obj->Leave(); }
};