#include "GlobalConfig.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

void CGlobalConfig::Read(const std::string& rstrJson)
{
	std::ifstream config(rstrJson);
	std::cout << rstrJson << ", " << config.is_open() << std::endl;
	json jc;
	config >> jc;
	m_strRemoteServer = jc["RemoteServer"];
	m_bLBState = jc["LBState"];
	m_bEnableRPC = jc["EnableRPC"];
	m_dwMaxSendMsgSize = jc["MaxSendMsgSize"];
	m_dwMaxRecvMsgSize = jc["MaxRecvMsgSize"];
	m_dwFallbackTimeout = jc["FallbackTimeout"];

	std::cout <<  m_strRemoteServer << std::endl;
	std::cout <<  m_bLBState << std::endl;
	std::cout <<  m_bEnableRPC << std::endl;
	std::cout <<  m_dwMaxSendMsgSize << std::endl;
	std::cout <<  m_dwMaxRecvMsgSize << std::endl;
	std::cout <<  m_dwFallbackTimeout << std::endl;
}

void CGlobalConfig::Read(const char* szJson)
{
	std::string json(szJson);
	Read(json);
}