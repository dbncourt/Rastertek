////////////////////////////////////////////////////////////////////////////////
// Filename: Timer.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TIMER_H_
#define _TIMER_H_


//////////////
// INCLUDES //
//////////////
#include <windows.h>

#pragma once
class Timer
{
private:
	INT64 m_frequency;
	FLOAT m_ticksPerMs;
	INT64 m_startTime;
	FLOAT m_frameTime;

public:
	Timer();
	Timer(const Timer& other);
	~Timer();

	bool Intialize();
	void Frame();

	float GetTime();
};

#endif