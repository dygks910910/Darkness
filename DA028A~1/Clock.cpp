#include "stdafx.h"
#include "Clock.h"



Clock::Clock()
{
	restTime = 100;
}


Clock::~Clock()
{
}
/*
2016 / 11 / 19 / 22:26
�ۼ���:�ڿ���(dygks910910@daum.net)
����:���۽ð�����ϰ� time_point�� ��ȯ.
*/

double Clock::getElapsedTime()
{
	std::chrono::high_resolution_clock::time_point nowtime = std::chrono::high_resolution_clock::now();
	std::chrono::milliseconds mill = std::chrono::duration_cast<std::chrono::milliseconds>(nowtime - startTime);
	return mill.count();
}


int Clock::getElapsedTimeSec()
{
	std::chrono::high_resolution_clock::time_point nowtime = std::chrono::high_resolution_clock::now();
	std::chrono::seconds mill = std::chrono::duration_cast<std::chrono::seconds>(nowtime - startTime);
	return mill.count();
}

void Clock::startTimer()
{
	startTime = std::chrono::high_resolution_clock::now();
}

const int& Clock::countDown(const int& countSec)
{
	restTime = countSec - getElapsedTimeSec();
	if (countSec - getElapsedTimeSec() <= 0)
		return 0; 
	return countSec - getElapsedTimeSec();
}

