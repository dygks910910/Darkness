#pragma once


class Clock
{

public:
	mutex lock;
private:
	std::chrono::high_resolution_clock::time_point startTime;
	std::chrono::high_resolution_clock::time_point endTime;
	std::chrono::high_resolution_clock::time_point preFrametime;


public:
	Clock::Clock();
	int restTime;//남은시간(countDown전용).

	~Clock();

	typedef std::chrono::duration<std::chrono::milliseconds> milsec;

	double getElapsedTime(); //지금까지 흐른 시간 리턴millisec
	int getElapsedTimeSec();
	void startTimer();
	const int& countDown(const int& countSec);

	void SetEndTime() { endTime = chrono::high_resolution_clock::now(); }
	//double GetDeltaTime();
};