#include "stdafx.h"
#include "Timer.h"



#include "ThreadFunctions.h"

Timer* Timer::instance = nullptr;

Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::AddTimer(const UINT id, const int type, const UINT time)
{
	Lock sync(this);

	NPC_EVENT new_event;
	new_event.id = id;
	new_event.type = type;
	new_event.time = GetCurrTime() + time;

	event_queue.push(new_event);
}


UINT Timer::GetCurrTime()
{
	static auto start = high_resolution_clock::now();

	return duration_cast<milliseconds>(high_resolution_clock::now() - start).count();
}


void Timer::TimerThread()
{
	auto timer = GetInstance();
	while (true)
	{
		Sleep(1);


		timer->Enter();
		if (timer->event_queue.empty())
		{
			timer->Leave();
			continue;
		}

		NPC_EVENT top_event = timer->event_queue.top();

		while (top_event.time <= GetCurrTime())
		{
			timer->event_queue.pop();
			timer->Leave();

			ProcessEvent(top_event);

			timer->Enter();
			if (timer->event_queue.empty())
			{
				timer->Leave();
				break;
			}

			top_event = timer->event_queue.top();
		}

		timer->Leave();

	}

	cout << " exit timer thread " << endl;
}
