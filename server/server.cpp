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

using fileutil::UncompressService;
using fileutil::UncompressReq;
using fileutil::UncompressRes;

class CompressServiceImpl final 
	: public CompressService::Service
{
	Status s_compress(ServerContext* context, ServerReaderWriter<CompressRes, CompressReq>* stream) override
	{
		std::cout << __FUNCTION__ << std::endl;
		std::unique_ptr<CompressReq> compressReq = std::unique_ptr<CompressReq>(new CompressReq());

		while(stream->Read(compressReq.get()))
		{
			const std::string& rstrSource = compressReq->source();
			char* szBuff = new char[rstrSource.length()];
			//设置source上限为10M
			if(rstrSource.length() >= (10 << 20))
			{
				//如果单次传输超过10M，返回错误，并取消本次压缩
				delete[] szBuff;
				return Status::CANCELLED;
			}
			else
			{
				std::unique_ptr<CompressRes> compressRes = std::unique_ptr<CompressRes>(new CompressRes());
				int compressLen = 0;
				std::cout << "start compressing" << std::endl;
				memset(szBuff, '\0', rstrSource.length());
				compress2((Bytef*)szBuff, (uLongf*)&compressLen, (const Bytef*)rstrSource.c_str(), (uLong)rstrSource.length(), Z_DEFAULT_COMPRESSION);
				std::cout << "finish compress" << std::endl;
				std::cout << compressLen << ", " << rstrSource.length() << std::endl;
				compressRes->set_compressed(std::string(szBuff, compressLen));
				std::cout << compressRes->compressed().length() << std::endl;
				stream->Write(*(compressRes.get()));
				std::cout << "response" << std::endl;
				delete[] szBuff;
				return Status::OK;
			}
		}
		return Status::OK;
	}
};

class UncompressServiceImpl final
	: public UncompressService::Service
{
	Status s_uncompress(ServerContext* context, ServerReaderWriter<UncompressRes, UncompressReq>* stream) override
	{
		std::cout << __FUNCTION__ << std::endl;
		std::unique_ptr<UncompressReq> uncompressReq = std::unique_ptr<UncompressReq>(new UncompressReq());
		while(stream->Read(uncompressReq.get()))
		{
			const std::string& rstrCompressed = uncompressReq->compressed();
			char* szBuff = new char[rstrCompressed.length() * 10];
			if(rstrCompressed.length() >= (10 << 20))
			{
				//如果单次传输超过10M，返回错误，并取消本次压缩
				delete[] szBuff;
				return Status::CANCELLED;
			}
			else
			{
				std::unique_ptr<UncompressRes> uncompressRes = std::unique_ptr<UncompressRes>(new UncompressRes());
				int uncompressLen = 0;
				std::cout << "start uncompressing" << std::endl;
				memset(szBuff, '\0', rstrCompressed.length() * 10);
				uncompress((Bytef*)szBuff, (uLongf*)&uncompressLen, (const Bytef*)rstrCompressed.c_str(), (uLong)rstrCompressed.length());
				std::cout << "finish uncompress" << std::endl;
				std::cout << uncompressLen << ", " << rstrCompressed.length() << std::endl;

				uncompressRes->set_uncompressed(szBuff, uncompressLen);
				std::cout << uncompressRes->uncompressed().length() << std::endl;
				stream->Write(*(uncompressRes.get()));
				std::cout << "write response done" << std::endl;

				delete[] szBuff;
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
