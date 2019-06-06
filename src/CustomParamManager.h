#pragma once
#include "Singleton.h"
#include "GlobalConfig.h"
class CCustomParamManager : public CSingleton<CCustomParamManager>
{
public:
	friend CSingleton;
	void Set(uint32_t dwCpuCore, uint64_t ullBuffSize)
	{
		m_dwCpuCore = dwCpuCore;
		m_ullBuffSize = ullBuffSize;
	}

	inline uint32_t GetCpuCore()
	{
		return m_dwCpuCore;
	}

	inline uint64_t GetBuffSize()
	{
		if(CGlobalConfig::Instance().IsEnableRPC())
		{
			//如果启用rpc，默认使用1M空间
			return 1 << 20;
		}
		else
		{
			return m_ullBuffSize;	
		}
	}

private:
	uint32_t m_dwCpuCore = 1;
	uint64_t m_ullBuffSize = (1 << 20);
	
private:
	CCustomParamManager() = default;
	~CCustomParamManager() = default;
};