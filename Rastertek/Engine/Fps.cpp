///////////////////////////////////////////////////////////////////////////////
// Filename: Fps.cpp
///////////////////////////////////////////////////////////////////////////////
#include "Fps.h"


Fps::Fps()
{
}

Fps::Fps(const Fps& other)
{
}


Fps::~Fps()
{
}

void Fps::Initialize()
{
	this->m_fps = 0;
	this->m_count = 0;
	this->m_startTime = 0;
}

void Fps::Frame()
{
	this->m_count++;
	if (timeGetTime() >= (this->m_startTime + 1000))
	{
		this->m_fps = this->m_count;
		this->m_count = 0;

		this->m_startTime = timeGetTime();
	}
}

int Fps::GetFps()
{
	return this->m_fps;
}