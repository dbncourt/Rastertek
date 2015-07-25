#include "Timer.h"


Timer::Timer()
{
}

Timer::Timer(const Timer& other)
{
}


Timer::~Timer()
{
}

bool Timer::Intialize()
{
	//Check to see if this system supports high performance timers
	QueryPerformanceFrequency((LARGE_INTEGER*)&this->m_frequency);
	if (this->m_frequency == 0)
	{
		return false;
	}

	//Find out how many times the frequency counter ticks every millisecond
	this->m_ticksPerMs = (float)(this->m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&this->m_startTime);

	return true;
}

void Timer::Frame()
{
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	timeDifference = (float)(currentTime - this->m_startTime);
	this->m_frameTime = timeDifference / this->m_ticksPerMs;
	this->m_startTime = currentTime;
}

float Timer::GetTime()
{
	return this->m_frameTime;
}