#pragma once
#include "SyncObject.h"

class Timer
	: public SyncObject
{
	priority_queue<NPC_EVENT, vector<NPC_EVENT>, Comparison> event_queue;

public:
	static void TimerThread();
	static UINT GetCurrTime();

public:
	void AddTimer(const UINT id, const int type, const UINT time);

private:
	static Timer* instance;

	Timer();
	virtual ~Timer();

public:
	static Timer* GetInstance()
	{
		if (instance == nullptr)
			instance = new Timer();

		return instance;
	}

	void DestroyInstance()
	{
		if (instance)
		{
			delete instance;
			instance = nullptr;
		}
	}
};

