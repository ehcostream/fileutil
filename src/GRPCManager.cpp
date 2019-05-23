#include "GRPCManager.h"
#include "GlobalConfig.h"

#define REMOTE_SERVICE "0.0.0.0:8000"
#define LOAD_BALANCE "round_robin"
#define FALL_BACK_TIMEOUT (10 * 1000)
#define MAX_SEND_MSG_SIZE (2 << 20)
#define MAX_RECV_MSG_SIZE (10 << 20)

std::shared_ptr<::grpc::Channel> CGRPCManager::GetChannel()
{
	if(m_stChannel.get() == nullptr)
	{
		::grpc::ChannelArguments stArgs;
		stArgs.SetCompressionAlgorithm(GRPC_COMPRESS_DEFLATE);

		if(CGlobalConfig::Instance().GetLoadBalanceState())
		{
			stArgs.SetLoadBalancingPolicyName(LOAD_BALANCE);
		}
		
		stArgs.SetGrpclbFallbackTimeout(CGlobalConfig::Instance().GetFallbackTimeOut());
		stArgs.SetMaxSendMessageSize(CGlobalConfig::Instance().GetMaxSendMsgSize());
		stArgs.SetMaxReceiveMessageSize(CGlobalConfig::Instance().GetMaxRecvMsgSize());

		m_stChannel = grpc::CreateCustomChannel(CGlobalConfig::Instance().GetRemoteServer(), grpc::InsecureChannelCredentials(), stArgs);
		if(m_stChannel.get() == nullptr)
		{
			std::cout << "Failed to login the remote service" << std::endl;
		}
		std::cout << "Create channel successfully" << std::endl;
	}
	return m_stChannel;
}
