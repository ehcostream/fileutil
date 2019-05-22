#pragma once 

#include <zlib.h>
#include <memory>
#include <grpcpp/grpcpp.h>
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



class CCompressService final : public CompressService::Service
{
	virtual Status s_compress(ServerContext* context, ServerReaderWriter<CompressRes, CompressReq>* stream) override
	{
		std::cout << __FUNCTION__ << std::endl;
		std::unique_ptr<CompressReq> compressReq = std::unique_ptr<CompressReq>(new CompressReq());
		Status status = Status::OK;
		uint32_t dwBuffSize = 1 << 20;
		char* szBuff = new char[dwBuffSize];
		uint64_t ullTotalCompressedSize = 0;
		while(stream->Read(compressReq.get()))
		{
			//设置source上限为1M
			if(compressReq->source().length() > (1 << 20))
			{
				//如果单次传输超过1M，返回错误，并取消本次压缩
				status = Status::CANCELLED;
				break;
			}
			else
			{
				
				std::unique_ptr<CompressRes> compressRes = std::unique_ptr<CompressRes>(new CompressRes());
				int compressLen = dwBuffSize;
				std::cout << "start compressing" << std::endl;
				memset(szBuff, '\0', 1 << 20);
				std::cout << "source len:" << compressReq->source().length() << std::endl;
				int err;
				err = compress2((Bytef*)szBuff, (uLongf*)&compressLen, (const Bytef*)compressReq->source().c_str(), (uLong)compressReq->source().length(), Z_DEFAULT_COMPRESSION);

				std::cout << "compress result:" << err << std::endl;			
				std::cout << "finish compress" << std::endl;
				std::cout << compressLen << ", " << compressReq->source().length() << std::endl;
				ullTotalCompressedSize += compressLen;
				compressRes->set_compressed(szBuff, compressLen);
				std::cout << compressRes->compressed().length() << std::endl;
				stream->Write(*compressRes);
				std::cout << "response" << std::endl;
				status = Status::OK;
			}
		}
		std::cout << "total compressed size : " << ullTotalCompressedSize << std::endl; 
		delete[] szBuff;
		return status;
	}
};

class CUncompressService final : public UncompressService::Service
{
	virtual Status s_uncompress(ServerContext* context, ServerReaderWriter<UncompressRes, UncompressReq>* stream) override
	{
		std::cout << __FUNCTION__ << std::endl;
		std::unique_ptr<UncompressReq> uncompressReq = std::unique_ptr<UncompressReq>(new UncompressReq());
		Status status = Status::OK;
		char* szBuff = new char[(10 << 20)];
		while(stream->Read(uncompressReq.get()))
		{
			if(uncompressReq->compressed().length() >= (10 << 20))
			{
				//如果单次传输超过10M，返回错误，并取消本次压缩
				status = Status::CANCELLED;
				break;
			}
			else
			{
				std::unique_ptr<UncompressRes> uncompressRes = std::unique_ptr<UncompressRes>(new UncompressRes());
				int uncompressLen = (10 << 20);
				std::cout << "start uncompressing" << std::endl;
				memset(szBuff, '\0', 10 << 20);
				uncompress((Bytef*)szBuff, (uLongf*)&uncompressLen, (const Bytef*)uncompressReq->compressed().c_str(), (uLong)uncompressReq->compressed().length());
				std::cout << "finish uncompress" << std::endl;
				std::cout << uncompressLen << ", " << uncompressReq->compressed().length() << std::endl;

				uncompressRes->set_uncompressed(szBuff, uncompressLen);
				std::cout << uncompressRes->uncompressed().length() << std::endl;
				stream->Write(*(uncompressRes.get()));
				std::cout << "write response done" << std::endl;

				status = Status::OK;
			}
		}
		delete[] szBuff;
		return status;
	}
};

class CEncodeService final : public