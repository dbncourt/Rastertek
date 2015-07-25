///////////////////////////////////////////////////////////////////////////////
// Filename: Cpu.cpp
///////////////////////////////////////////////////////////////////////////////
#include "Cpu.h"


Cpu::Cpu()
{
}

Cpu::Cpu(const Cpu& other)
{
}


Cpu::~Cpu()
{
}

void Cpu::Initialize()
{
	PDH_STATUS status;

	//Initialize the flag indicating whether this object can read the system CPU usage or not
	this->m_canReadCpu = true;

	//Create a query object to pool CPU usage
	status = PdhOpenQuery(nullptr, 0, &this->m_queryHandle);
	if (status != ERROR_SUCCESS)
	{
		this->m_canReadCpu = false;
	}

	//Set query object to pool CPU's in the system
	status = PdhAddCounter(this->m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &this->m_counterHandle);
	if (status != ERROR_SUCCESS)
	{
		this->m_canReadCpu = false;
	}

	this->m_lastSampleTime = GetTickCount();
	this->m_cpuUsage = 0;
}

void Cpu::Shutdown()
{
	if (this->m_canReadCpu)
	{
		PdhCloseQuery(this->m_queryHandle);
	}
}

void Cpu::Frame()
{
	PDH_FMT_COUNTERVALUE value;

	if (this->m_canReadCpu)
	{
		if ((this->m_lastSampleTime + 1000) < GetTickCount())
		{
			this->m_lastSampleTime = GetTickCount();
			PdhCollectQueryData(this->m_queryHandle);
			PdhGetFormattedCounterValue(this->m_counterHandle, PDH_FMT_LONG, nullptr, &value);
			this->m_cpuUsage = value.longValue;
		}
	}
}

int Cpu::GetCpuPercentage()
{
	int usage;

	if (this->m_canReadCpu)
	{
		usage = (int)this->m_cpuUsage;
	}
	else
	{
		usage = 0;
	}

	return usage;
}