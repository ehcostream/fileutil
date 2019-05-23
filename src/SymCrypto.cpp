#include "SymCrypto.h"
#include "FileUtilHead.h"
#include <grpcpp/grpcpp.h>
#include "fileutil.grpc.pb.h"
#include "GRPCManager.h"
#include <Constants.h>
#include "GlobalConfig.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using fileutil::CryptoService;
using fileutil::CryptoReq;
using fileutil::CryptoRes;


int CSymCrypto::SymEncode(const std::string& rstrSource, const std::string& rstrOut, const std::string& rstrKey, bool bEncode, std::string& rstrOutFile)
{
    if(CGlobalConfig::Instance().IsEnableRPC())
    {
        return SymEncodeWithGrpc(rstrSource, rstrOut, rstrKey, bEncode, rstrOutFile);
    }
    //判断输出文件夹是否存在如果不存在，则创建
    std::cout << __FILE__ << " " << __FUNCTION__ << " " << rstrKey << std::endl;
    fs::path filePath(rstrOut);
    if(!fs::exists(filePath))
    {
        fs::create_directories(filePath);
    }
    
    std::ifstream in(rstrSource, std::ifstream::in | std::ifstream::binary);
    std::ofstream out;
    std::string strExtension(".spc");
    int nError = 0;
    do
    {
        if(!bEncode)
        {
            //如果是解密，首先解析头信息，取出真实密码并判断密码与文件本身使用的密码是否一致
            //如果不一致，直接返回，密码错误
            //如果一致，使用密码对文件内容进行解密
            int nParseResult = 0;
            FileHead stHead;
            std::string strRealKey = CFileUtilHead::Parse(in, nParseResult, stHead);
            if(nParseResult != 0 || std::string(stHead.szExt) != strExtension)
            {
                //文件格式无效
                nError = 1;
                break;
            }
            std::cout << strRealKey << "<= =>" << rstrKey << std::endl;
            if(strRealKey != rstrKey)
            {
                //密码错误
                nError = 2;
                break;
            }
            rstrOutFile = (filePath / std::string(stHead.szFilename)).string();
        }
        else
        {
            
            int nParseResult = 0;
            FileHead stHead;
            std::string strRealKey = CFileUtilHead::Parse(in, nParseResult, stHead);
            if(std::string(stHead.szExt) == strExtension)
            {
                //重复加密
                nError = 3;
                break;
            }
            else
            {
                //恢复文件指针
                rstrOutFile = fs::path(rstrSource).filename().string();
                rstrOutFile.append(strExtension);
                rstrOutFile = (filePath / rstrOutFile).string();
                in.seekg(0, in.beg);
            }

        }

        std::cout << "source:" << rstrSource << "out:" << rstrOutFile << std::endl;
        out.open(rstrOutFile, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        if(!out.is_open())
        {
            nError = 4;
            break;
        }
        if(bEncode)
        {
            //附加文件头信息
            CFileUtilHead::Attach(out, rstrOutFile, rstrKey);
        }
        uint64_t dwPos = 0;
        char c;
        //开始进行解密/解密
        while(in.peek() != EOF)
        {
            char szXor = rstrKey.at((dwPos++) % rstrKey.length());
            in.get(c);
            char tC = szXor ^ (char)c;
            out << tC;
        }

    }while(false);
    in.close();
    out.close();
    return nError;
}

int CSymCrypto::SymEncodeWithGrpc(const std::string& rstrSource, const std::string& rstrOut, const std::string& rstrKey, bool bEncode, std::string& rstrOutFile)
{
    std::cout << __FILE__ << " " << __FUNCTION__ << " " << rstrKey << std::endl;
    fs::path filePath(rstrOut);
    if(!fs::exists(filePath))
    {
        fs::create_directories(filePath);
    }

    std::shared_ptr<Channel> channel = CGRPCManager::Instance().GetChannel();
    std::unique_ptr<CryptoService::Stub> stub = std::unique_ptr<CryptoService::Stub>(CryptoService::NewStub(channel));

    std::ifstream in(rstrSource, std::ifstream::in | std::ifstream::binary);
    std::ofstream out;
    std::string strExtension(".spc");
    int nError = 0;
    do
    {
        if(!bEncode)
        {
            //如果是解密，首先解析头信息，取出真实密码并判断密码与文件本身使用的密码是否一致
            //如果不一致，直接返回，密码错误
            //如果一致，使用密码对文件内容进行解密
            int nParseResult = 0;
            FileHead stHead;
            std::string strRealKey = CFileUtilHead::Parse(in, nParseResult, stHead);
            if(nParseResult != 0 || std::string(stHead.szExt) != strExtension)
            {
                //文件格式无效
                nError = 1;
                break;
            }
            std::cout << strRealKey << "<= =>" << rstrKey << std::endl;
            if(strRealKey != rstrKey)
            {
                //密码错误
                nError = 2;
                break;
            }
            rstrOutFile = (filePath / std::string(stHead.szFilename)).string();
        }
        else
        {
            
            int nParseResult = 0;
            FileHead stHead;
            std::string strRealKey = CFileUtilHead::Parse(in, nParseResult, stHead);
            if(std::string(stHead.szExt) == strExtension)
            {
                //重复加密
                nError = 3;
                break;
            }
            else
            {
                //恢复文件指针
                rstrOutFile = fs::path(rstrSource).filename().string();
                rstrOutFile.append(strExtension);
                rstrOutFile = (filePath / rstrOutFile).string();
                in.seekg(0, in.beg);
            }

        }

        std::cout << "source:" << rstrSource << "out:" << rstrOutFile << std::endl;
        out.open(rstrOutFile, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        if(!out.is_open())
        {
            nError = 4;
            break;
        }

        ClientContext context;
        auto stream = stub->s_crypto(&context);
        if(bEncode)
        {
            //附加文件头信息
            CFileUtilHead::Attach(out, rstrOutFile, rstrKey);
        }

        uint32_t dwPos = 0;
        std::string strEncodeKey;
        for(const auto c : rstrKey)
        {
            char szXor = Constants::ENCODE_KEY.at((dwPos++) % Constants::ENCODE_KEY.length());
            char tC = szXor ^ (char)c;
            strEncodeKey.append(&tC, 1);
        }

        //通过rpc进行加密或者解密
        std::unique_ptr<CryptoReq> cryptoReq = std::unique_ptr<CryptoReq>(new CryptoReq());
        std::unique_ptr<CryptoRes> cryptoRes = std::unique_ptr<CryptoRes>(new CryptoRes());

        //首次传输发送密码 
        cryptoReq->set_source(strEncodeKey);
        stream->Write(*cryptoReq);
        stream->Read(cryptoRes.get());

        cryptoReq->clear_source();
        cryptoRes->clear_result();

        char* szBuff = new char[ 1 << 20 ];
        while(in.peek() != EOF)
        {
            in.read(szBuff, 1 << 20);
            int cnt = in.gcount();
            cryptoReq->set_source(szBuff, cnt);

            stream->Write(*cryptoReq);
            stream->Read(cryptoRes.get());
            out << cryptoRes->result();
        }
        stream->WritesDone();
        Status status = stream->Finish();
        if(!status.ok())
        {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        }
        delete[] szBuff;

    }while(false);
    in.close();
    out.close();
    return nError;
}