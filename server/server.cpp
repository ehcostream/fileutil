#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>
#include <zlib.h>

#include "fileutil.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

using fileutil::CompressService;
using fileutil::CompressReq;
using fileutil::CompressRes;

class CompressServiceImpl final 
	: public CompressService::Service
{
	Status s_compress(ServerContext* context, ServerReaderWriter<CompressRes, CompressReq>* stream) override
	{
		std::unique_ptr<CompressReq> compressReq = std::unique_ptr<CompressReq>(new CompressReq());

		while(stream->Read(compressReq.get()))
		{
			const std::string& rstrSource = compressReq->source();
			//设置source上线为10M，
			if(rstrSource.length() >= (10 << 20))
			{
				//如果单次传输超过10M，返回错误，并取消本次压缩
				return Status::CANCELLED;
			}
			else
			{
				std::unique_ptr<CompressRes> compressRes = std::unique_ptr<CompressRes>(new CompressRes());
				std::string& rstrCompressed = *(compressRes->mutable_compressed());
				int compressLen = 0;
				compress((Bytef*)rstrCompressed.c_str(), (uLongf*)&compressLen, (const Bytef*)rstrSource.c_str(), (uLong)rstrSource.length());
				stream->Write(*compressRes);
				return Status::OK;
			}
		}
		return Status::OK;
	}
};

void RunServer()
{
	std::string server_address("0.0.0.0:8000");
	CompressServiceImpl compressService;
	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&compressService);
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server Listening On " << server_address << std::endl;

	server->Wait();
}

int main()
{
	RunServer();
	return 0;
}
