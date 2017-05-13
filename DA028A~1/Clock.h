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
	int restTime;//�����ð�(countDown����).

	~Clock();

	typedef std::chrono::duration<std::chrono::milliseconds> milsec;

	double getElapsedTime(); //���ݱ��� �帥 �ð� ����millisec
	int getElapsedTimeSec();
	void startTimer();
	const int& countDown(const int& countSec);

	void SetEndTime() { endTime = chrono::high_resolution_clock::now(); }
	//double GetDeltaTime();
};