#ifndef __GRPC_MANAGER_H__
#define __GRPC_MANAGER_H__
#include "Singleton.h"
#include <grpcpp/grpcpp.h>
#include <memory>

class CGRPCManager : public CSingleton<CGRPCManager>
{
public:
	friend CSingleton;

	std::shared_ptr<::grpc::Channel> GetChannel();

private:
	std::shared_ptr<::grpc::Channel> m_stChannel = nullptr;

private:
	CGRPCManager() = default;
	~CGRPCManager() = default;
};
#endif