#pragma once
#include "Singleton.h"
class CCustomParamManager : public CSingleton<CCustomParamManager>
{
public:
	void Set(uint32_t dwCpuCore, uint64_t ullBuffSize)
	{
		m_dwCpuCore = dwCpuCore;
		m_ullBuffSize = ullBuffSize;
	}

	uint32_t GetCpuCore()
	{
		return m_dwCpuCore;
	}

	uint64_t GetBuffSize()
	{
		return m_ullBuffSize;
	}

private:
	uint32_t m_dwCpuCore = 1;
	uint64_t m_ullBuffSize = (1 << 20);
};