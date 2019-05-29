#include "GlobalConfig.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <fstream>
#include <iostream>

using namespace boost::property_tree;

void CGlobalConfig::Read(const std::string& rstrJson)
{
	std::ifstream json(rstrJson);
	ptree pt;
	try
	{
		read_json(json, pt);
	}
	catch(ptree_error& e)
	{
		std::cout << "-----------------ptree error----------------" << std::endl;
	}
	m_strRemoteServer = pt.get<std::string>("RemoteServer");
	m_bEnableLoadBalance = pt.get<bool>("EnableLoadBalance");
	m_bEnableRPC = pt.get<bool>("EnableRPC");
	m_dwMaxSendMsgSize = pt.get<int>("MaxSendMsgSize");
	m_dwMaxRecvMsgSize = pt.get<int>("MaxRecvMsgSize");
	m_dwFallbackTimeout = pt.get<int>("FallbackTimeout");

	std::cout <<  m_strRemoteServer << std::endl;
	std::cout <<  m_bEnableLoadBalance << std::endl;
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