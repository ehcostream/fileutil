#ifndef __GLOBAL_CONFIG_H__
#define __GLOBAL_CONFIG_H__
#include "Singleton.h"
#include <string>


class CGlobalConfig : public CSingleton<CGlobalConfig>
{
public:
	friend CSingleton;
	void Read(const std::string& rstrJson);
	void Read(const char* szJson);

public:
	std::string GetRemoteServer() { return m_strRemoteServer; }
	bool GetLoadBalanceState() { return m_bEnableLoadBalance; }
	bool IsEnableRPC() { return m_bEnableRPC; }
	uint32_t GetMaxSendMsgSize() { return m_dwMaxSendMsgSize; }
	uint32_t GetMaxRecvMsgSize() { return m_dwMaxRecvMsgSize; }
	uint32_t GetFallbackTimeOut() { return m_dwFallbackTimeout; }

private:
	std::string m_strRemoteServer;
	bool m_bEnableLoadBalance = false;
	bool m_bEnableRPC = false;
	uint32_t m_dwMaxSendMsgSize = (2 << 20);
	uint32_t m_dwMaxRecvMsgSize = (10 << 20);
	uint32_t m_dwFallbackTimeout = (10 * 1000);

private:
	CGlobalConfig() = default;
	~CGlobalConfig() = default;
};

#endif