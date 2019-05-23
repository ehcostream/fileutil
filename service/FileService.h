#pragma once 

#include <zlib.h>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "fileutil.grpc.pb.h"
#include <fz/Constants.h>

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

using fileutil::CryptoService;
using fileutil::CryptoReq;
using fileutil::CryptoRes;


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

class CCryptoService final : public CryptoService::Service
{
	virtual Status s_crypto(ServerContext* context, ServerReaderWriter<CryptoRes, CryptoReq>* stream) override
	{
		std::cout << __FUNCTION__ << std::endl;

		std::string strBuff;
		Status status = Status::OK;
		std::unique_ptr<CryptoReq> cryptoReq = std::unique_ptr<CryptoReq>(new CryptoReq());
		std::unique_ptr<CryptoRes> cryptoRes = std::unique_ptr<CryptoRes>(new CryptoRes());
		std::string strUserKey;
		while(stream->Read(cryptoReq.get()))
		{
			if(cryptoReq->source().length() > ( 1 << 20) )
			{
				return Status::CANCELLED;
			}
			else
			{
				//第一次收到密钥，默认密钥都是密文，在服务端还原成明文
				if(strUserKey.empty())
				{
					strUserKey = cryptoReq->source();
					std::cout << "encoded key： " << strUserKey << std::endl; 
					uint32_t dwPos = 0;

					std::string strEncodeKey;
					for(const auto c : strUserKey)
					{
						char szXor = Constants::ENCODE_KEY.at((dwPos++) % Constants::ENCODE_KEY.length());
						char tC = szXor ^ (char)c;
						strEncodeKey.append(&tC, 1);
					}
					strUserKey = strEncodeKey;
					std::cout << "decoded key： " << strUserKey << std::endl;
				}

				//加密字节
				uint64_t dwPos = 0;
				for(const char byte : cryptoReq->source())
				{
					char szXor = strUserKey.at((dwPos++) % strUserKey.length());
			        char result = szXor ^ byte;
			        strBuff.append(&result, 1);
				}

				cryptoRes->set_result(strBuff);
				stream->Write(*cryptoRes);
			}
		}

		return status;
	}
};
