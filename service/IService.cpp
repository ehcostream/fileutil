#include "IService.h"
#include "FileService.h"


::grpc::Service** CreateServices(int* count)
{
	//为服务申请堆空间
	ServicePool::Instance().Alloc<CCompressService>();
	ServicePool::Instance().Alloc<CUncompressService>();
	ServicePool::Instance().Alloc<CCryptoService>();

	//更新服务数目
	*count = ServicePool::Instance().GetPoolSize();

	//获取服务容器数据
	const std::vector<::grpc::Service*>& services = ServicePool::Instance().GetPoolData();

	//返回服务指针列表
	return (::grpc::Service**)&(services[0]);
}