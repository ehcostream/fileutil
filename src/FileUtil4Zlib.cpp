#include "FileUtil4Zlib.h"
#include "FileUtilHead.h"
#include "CustomParamManager.h"
#include "GRPCManager.h"
#include <grpcpp/grpcpp.h>
#include "fileutil.grpc.pb.h"
#include "GlobalConfig.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using fileutil::CompressReq;
using fileutil::CompressRes;
using fileutil::CompressService;

using fileutil::UncompressReq;
using fileutil::UncompressRes;
using fileutil::UncompressService;


//线程参数
struct ThreadParam
{
    int threadSeq;
    uint64_t ullBuffSize;
    std::string strSource;
    std::string strOutFile;
};

int CFileUtil4Zlib::Compress(const std::vector<std::string>& rVecFiles, const std::string& rstrOutDir, std::string& rstrOutFile)
{
    if(CGlobalConfig::Instance().IsEnableRPC())
    {
        return CompressWithGrpc(rVecFiles, rstrOutDir, rstrOutFile);
    }

    std::cout << __FUNCTION__ << std::endl;
    assert(rVecFiles.size() > 0);
    int nError = 0;
    do
    {
        if(fs::is_regular_file(rVecFiles.front()) && rVecFiles.size() == 1)
        {
            std::unique_ptr< std::ifstream > ifsp = std::unique_ptr< std::ifstream >(new std::ifstream(rVecFiles.front()));
            FileHead stHead;
            int nResult = 0;
            CFileUtilHead::Parse(*ifsp, nResult, stHead);
            if(strcmp(stHead.szExt, ".zb") == 0)
            {
                ifsp->close();
                nError = 0;
                break;
            }
            ifsp->close();
        }

        std::string strArchiveFile;
        GetTmpMiddleFile(strArchiveFile, true);
        assert(!strArchiveFile.empty());
        nError = Archive(rVecFiles, strArchiveFile);
        PrepareCompress(rVecFiles, rstrOutDir, rstrOutFile);

        if(CCustomParamManager::Instance().GetCpuCore() > 1)
        {
            nError = CompressWithMT(strArchiveFile, rstrOutFile);
        }
        else
        {
            //调用zlib进行压缩
            nError = CompressFileWithZlib(strArchiveFile, rstrOutFile, CCustomParamManager::Instance().GetBuffSize());
        }

        //删除临时归档文件
        fs::remove(strArchiveFile);

    }while(false);

    return nError;
}


int CFileUtil4Zlib::CompressWithGrpc(const std::vector<std::string>& rVecFiles, const std::string& rstrOutDir, std::string& rstrOutFile)
{
    std::cout << __FUNCTION__ << std::endl;
    assert(rVecFiles.size() > 0);
    int nError = 0;
    
    do
    {
        if(fs::is_regular_file(rVecFiles.front()) && rVecFiles.size() == 1)
        {
            int nParseResult = 0;
            std::unique_ptr< std::ifstream > ifsp = std::unique_ptr< std::ifstream >(new std::ifstream(rVecFiles.front()));
            std::istream * isp = ifsp.get();
            FileHead stHead;
            CFileUtilHead::Parse(*isp, nParseResult, stHead);
            if(std::string(stHead.szExt) == std::string(".zb"))
            {
                ifsp->close();
                //重复压缩
                nError = 10;
                break;
            }
            ifsp->close();
        }

        //归档
        std::string strMidFile;
        GetTmpMiddleFile(strMidFile, true);
        assert(!strMidFile.empty());
        nError = Archive(rVecFiles, strMidFile);
        PrepareCompress(rVecFiles, rstrOutDir, rstrOutFile);
            
        //rpc 初始化
        std::shared_ptr<Channel> channel = CGRPCManager::Instance().GetChannel();
        std::unique_ptr<CompressService::Stub> stub = std::unique_ptr<CompressService::Stub>(CompressService::NewStub(channel));
        std::unique_ptr<ClientContext> context = std::unique_ptr<ClientContext>(new ClientContext());
        //调用rpc压缩服务进行压缩
        auto stream = stub->s_compress(context.get());
        std::unique_ptr< std::ifstream > ifsp = std::unique_ptr< std::ifstream >(new std::ifstream(strMidFile, std::ifstream::binary));
        std::unique_ptr< std::ofstream > ofsp = std::unique_ptr< std::ofstream >(new std::ofstream(rstrOutFile, std::ofstream::binary));
        //为输出文件附加文件头
        CFileUtilHead::Attach(*ofsp, rstrOutFile);
        do
        {
            if(ifsp->is_open())
            {
                std::cout << "open successfully" << std::endl;

                char* szBuff = new char[CCustomParamManager::Instance().GetBuffSize() + 1];

                std::unique_ptr<CompressReq> spReq = std::unique_ptr<CompressReq>(new CompressReq());
                std::unique_ptr<CompressRes> spRes = std::unique_ptr<CompressRes>(new CompressRes());

                if(szBuff)
                {
                    while(ifsp->peek() != EOF)
                    {
                        spRes->clear_compressed();
                        ifsp->read(szBuff, CCustomParamManager::Instance().GetBuffSize());
                        std::streamsize cnt = ifsp->gcount();
                        szBuff[cnt] = '\0';
                        std::cout << "cnt:" << cnt << std::endl;
                        spReq->set_source(szBuff, cnt);
                        std::cout << "spBuff length:" << spReq->source().length() << std::endl;

                        stream->Write(*spReq);
                        stream->Read(spRes.get());
                        std::cout << "compressed:" << spRes->compressed().length() << std::endl;
                        int zlibblocklength = spRes->compressed().length();
                        //每个zlib块都记录长度
                        ofsp->write((char*)&zlibblocklength, sizeof(zlibblocklength));
                        ofsp->write(spRes->compressed().c_str(), spRes->compressed().length());
                    }
                    stream->WritesDone();
                    Status status = stream->Finish();
                    if(!status.ok())
                    {
                        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
                    }
                }
                else
                {
                    nError = 11;
                    break;
                }
                delete[] szBuff;
            }
            else
            {
                std::cout << "open failed" << std::endl;
                nError = 10;
                break;
            }
        }while(false);
        ifsp->close();
        ofsp->close();
        fs::remove(fs::path(strMidFile));
    
    }while(false);
    
    return nError;
}


int CFileUtil4Zlib::Uncompress(const std::string& rstrIn, const std::string& rstrOutDir)
{
    if(CGlobalConfig::Instance().IsEnableRPC())
    {
        return UncompressWithGrpc(rstrIn, rstrOutDir);
    }
    std::string strArchiveFile;
    GetTmpMiddleFile(strArchiveFile, false);
    assert(!strArchiveFile.empty());
    int nError = 0;
    nError = UncompressFileWithZlib(rstrIn, strArchiveFile);
    nError = Dearchive(strArchiveFile, rstrOutDir);
    fs::remove(fs::path(strArchiveFile));
    return nError;
}

int CFileUtil4Zlib::UncompressWithGrpc(const std::string& rstrIn, const std::string& rstrOutDirs)
{
    std::string strMidFile;
    GetTmpMiddleFile(strMidFile, false);
    assert(!strMidFile.empty());
    std::cout << __FUNCTION__ << std::endl;

    

    //文件进行解压缩
    int nError = 0;
    std::unique_ptr< std::ifstream > ifsp;
    std::unique_ptr< std::ofstream > ofsp;
    do
    {
        if(fs::is_directory(fs::path(rstrIn)))
        {
            std::cout << "file format is invalid" << std::endl;
            nError = 1;
            break;
        }

        //调用rpc进行解压缩
        std::shared_ptr<Channel> channel = CGRPCManager::Instance().GetChannel();
        std::unique_ptr<UncompressService::Stub> stub = std::unique_ptr<UncompressService::Stub>(UncompressService::NewStub(channel));
        
        ClientContext context;
        std::cout << "start calling rpc" << std::endl;
        auto stream = stub->s_uncompress(&context);
        std::cout << "call rpc uncompress" << std::endl;
        //初始化s_uncompress调用参数
        std::unique_ptr<UncompressReq> spReq = std::unique_ptr<UncompressReq>(new UncompressReq());
        std::unique_ptr<UncompressRes> spRes = std::unique_ptr<UncompressRes>(new UncompressRes());

        ifsp = std::unique_ptr< std::ifstream >(new std::ifstream(rstrIn, std::ifstream::binary));
        //解压后的ar文件临时保存路径
        ofsp = std::unique_ptr< std::ofstream >(new std::ofstream(strMidFile, std::ofstream::binary));

        if(fs::is_directory(fs::path(rstrIn)))
        {
            std::cout << "file format is invalid" << std::endl;
            nError = 2;
            break;   
        }
        FileHead stHead;
        //检测文件是否有效
        CFileUtilHead::Parse(*ifsp, nError, stHead);
        if(nError != 0)
        {
            nError = 3;
            break;
        }
        if(std::string(stHead.szExt) != std::string(".zb"))
        {
            nError = 4;
            break;
        }

        char* szBuff = new char[CCustomParamManager::Instance().GetBuffSize() + 1];
        if(szBuff)
        {
            while(ifsp->peek() != EOF)
            {
                //首先读取下一个zlib块的长度
                int zlibblocklength = 0;
                ifsp->read((char*)&zlibblocklength, sizeof(zlibblocklength));
                //读取zlib内容
                ifsp->read(szBuff, zlibblocklength);
                std::streamsize cnt = ifsp->gcount();
                szBuff[cnt] = '\0';

                spReq->set_compressed(szBuff, cnt);
                std::cout << "compressed file length:" << cnt << std::endl;
                stream->Write(*spReq);
                stream->Read(spRes.get());

                std::cout << "uncompressed file length:" << spRes->uncompressed().length() << std::endl;
                ofsp->write(spRes->uncompressed().c_str(), spRes->uncompressed().length());

            }
            stream->WritesDone();
            Status status = stream->Finish();
            if(!status.ok())
            {
                std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            }
        }
        else
        {
            ifsp->close();
            ofsp->close();
            nError = 2;
            break;
        }
        ifsp->close();
        ofsp->close();
        //解档
        nError = Dearchive(strMidFile, rstrOutDirs);
    }while(false);

    
    //删除临时文件
    fs::remove(fs::path(strMidFile));
    std::cout << __FUNCTION__ << ", end" << std::endl;
    return nError;
}

int CFileUtil4Zlib::CompressWithMT(const std::string rstrAchiveFile, const std::string& rstrOutDir)
{
    //切割为多个多文件
    //开启线程池，加入多个线程，根据buffsize平均分配，同时压缩文件
    //等待所有线程完成，将结果写入目标文件中
    assert(CCustomParamManager::Instance().GetBuffSize() > 0 && CCustomParamManager::Instance().GetCpuCore() > 1);

    uint32_t dwThreadCnt = CCustomParamManager::Instance().GetCpuCore();
    std::vector<std::string> vecFiles;
    std::vector<std::string> vecResultFiles;

    int nError = 0;
    do
    {
        
        nError = CutFileIntoPieces(rstrAchiveFile, vecFiles, dwThreadCnt);
        if(vecFiles.size() != dwThreadCnt)
        {
            std::cout << "vecfiles size invalid:" << vecFiles.size() << std::endl;;
            nError = 1;
            break;
        }

        std::vector<ThreadParam> vecThreadList;
        uint32_t dwGeneralAvg = floor(CCustomParamManager::Instance().GetBuffSize() / dwThreadCnt);
        boost::thread_group threadPool;
        //初始化线程池
        for(uint32_t i = 0; i< dwThreadCnt; i++)
        {
            ThreadParam stThreadParam;
            stThreadParam.threadSeq = i;
            if(i == dwThreadCnt -1)
            {
                stThreadParam.ullBuffSize = CCustomParamManager::Instance().GetBuffSize() - dwGeneralAvg * (dwThreadCnt - 1);
            }
            else
            {
                stThreadParam.ullBuffSize = dwGeneralAvg;
            }
            stThreadParam.strSource = vecFiles[i];
            vecThreadList.emplace_back(stThreadParam);
        }

        std::cout << "thread prepare..." << vecThreadList.size() << std::endl;
        //多线程同时压缩数据
        for(auto& param : vecThreadList)
        {
            auto threadCallBack = &CFileUtil4Zlib::CompressAFile;
            threadPool.create_thread(boost::bind(threadCallBack, this, std::ref(param)));
        }

        threadPool.join_all();    
        
        std::cout << "thread finished" << std::endl;
        //处理完成重组数据
        
        for(const auto& param : vecThreadList)
        {
            vecResultFiles.emplace_back(param.strOutFile);
        }

        std::cout << "ready to combain files" << std::endl;
        //重组数据
        CombainFiles(vecResultFiles, rstrOutDir);

        
    }while(false);
    //清理中间文件
    for(const auto& file : vecFiles)
    {
        fs::remove(fs::path(file));
    }
    for(const auto& file : vecResultFiles)
    {
        fs::remove(fs::path(file));
    }

    return nError;
}

void CFileUtil4Zlib::CompressAFile(ThreadParam& stParam)
{
    GetTmpMiddleFile(stParam.strOutFile, true, 2, stParam.threadSeq);
    std::cout << "  thread source--->" << stParam.strSource << "  thread result--->" << stParam.strOutFile << " buff size-->" << stParam.ullBuffSize << std::endl;
    CompressFileWithZlib(stParam.strSource, stParam.strOutFile, stParam.ullBuffSize, true);
    std::cout << "  thread result--->" << stParam.strOutFile << std::endl;
}


void CFileUtil4Zlib::PrepareCompress(const std::vector<std::string>& rVecFiles, const std::string& rstrOutDir, std::string& rstrOutFile)
{
    //对生成的压缩文件
    fs::path outFilePath;
    try
    {
        fs::path outDir(rstrOutDir);
        if(!fs::exists(outDir))
        {
            fs::create_directories(outDir);
        }
        outFilePath = fs::path(outDir);
        std::string strExtension = ".ar.zb";
        std::string strTargetFile;
        
        std::string strFront = rVecFiles.front();
        //如果压缩多个文件
        if(rVecFiles.size() > 1)
        {
            //如果包含多个文件，默认以第一个文件的父目录名称为文件名建立压缩文件
            if(strFront.back() == '/' || strFront.back() == '\\')
            {   
                strFront.pop_back();
            }
            strTargetFile = fs::canonical(fs::absolute(strFront).parent_path()).filename().string();
        }
        else
        {
            fs::path filePath(strFront);
            if(fs::is_regular_file(filePath))
            {
                strTargetFile = filePath.stem().string();
            }
            else if(fs::is_directory(filePath))
            {
                if(strFront.back() == '/' || strFront.back() == '\\')
                {   
                    strFront.pop_back();
                }
                filePath = fs::path(strFront);
                strTargetFile = filePath.filename().string();
            }
            else
            {
                //dont handle
            }
        }
        
        strTargetFile.append(strExtension);
        outFilePath /= strTargetFile;
        rstrOutFile = outFilePath.string();
        std::cout << "Output file path: " << outFilePath << std::endl;
    }
    catch(std::exception ext)
    {
        std::cout << ext.what() << std::endl;
    }
}


int CFileUtil4Zlib::CompressFileWithZlib(const std::string& rstrSource, const std::string& rstrCompressedFile, uint64_t ullBuffSize, bool bMT)
{
    int nError = 0;
    char* szSource = nullptr;
    char* szCompressed = nullptr;
    std::unique_ptr< std::ifstream > ifsp;
    std::unique_ptr< std::ofstream > ofsp;
    do
    {
        char* szSource = new char[ullBuffSize];
        if(szSource == nullptr)
        {
            nError = 1;
            break;
        }

        char* szCompressed = new char[ullBuffSize];
        if(szCompressed == nullptr)
        {
            nError = 2;
            break;
        }

        ifsp = std::unique_ptr< std::ifstream >(new std::ifstream(rstrSource, std::ifstream::binary));
        if(!ifsp->is_open())
        {
            nError = 2;
            break;
        }

        ofsp = std::unique_ptr< std::ofstream >(new std::ofstream(rstrCompressedFile, std::ofstream::binary));
        if(!ofsp->is_open())
        {
            nError = 3;
            break;
        }

        //为压缩文件附加头信息，多线程除外，多线程在最后重组时附加
        if(!bMT)
        {
            CFileUtilHead::Attach(*ofsp, rstrCompressedFile);    
        }
        
        while(ifsp->peek() != EOF)
        {
            ifsp->read(szSource, ullBuffSize);
            std::streamsize readBytes = ifsp->gcount();
            szSource[readBytes] = '\0';
            std::cout << "Read bytes: " << readBytes << std::endl;

            int err, compressLen = CCustomParamManager::Instance().GetBuffSize();
            err = compress2((Bytef*)szCompressed, 
                            (uLongf*)&compressLen, 
                            (const Bytef*)szSource, 
                            (uLong)readBytes, 
                            Z_DEFAULT_COMPRESSION);

            if(err != 0)
            {
                nError = 4;
                break;
            }
            std::cout << "Compressed len: " << compressLen << std::endl;
            //zlibheader + compressed content ... zlibheader + compressed content
            //每次压缩后记录压缩后长度并记录到文件中，以便于进行解压缩
            ofsp->write((char*)&compressLen, sizeof(compressLen));
            ofsp->write(szCompressed, compressLen);
        }

    }while(false);

    if(szSource) delete[] szSource;
    if(szCompressed) delete[] szCompressed;
    if(ifsp->is_open()) ifsp->close();
    if(ofsp->is_open()) ofsp->close();

    return nError;
}

int CFileUtil4Zlib::UncompressFileWithZlib(const std::string& rstrCompressedFile, const std::string& rstrSource)
{
    int nError = 0;
    char* szSource = nullptr;
    char* szCompressed = nullptr;
    std::unique_ptr< std::ifstream > ifsp;
    std::unique_ptr< std::ofstream > ofsp;
    do
    {
        if(fs::is_directory(fs::path(rstrCompressedFile)))
        {
            nError = 1;
            break;
        }

        char* szSource = new char[CCustomParamManager::Instance().GetBuffSize() * 10];
        if(szSource == nullptr)
        {
            nError = 2;
            break;
        }

        char* szCompressed = new char[CCustomParamManager::Instance().GetBuffSize()];
        if(szCompressed == nullptr)
        {
            nError = 3;
            break;
        }

        ifsp = std::unique_ptr< std::ifstream >(new std::ifstream(rstrCompressedFile, std::ifstream::binary));
        if(!ifsp->is_open())
        {
            nError = 4;
            break;
        }

        ofsp = std::unique_ptr< std::ofstream >(new std::ofstream(rstrSource, std::ofstream::binary));
        if(!ofsp->is_open())
        {
            nError = 5;
            break;
        }

        //解析头信息
        FileHead stHead;
        CFileUtilHead::Parse(*ifsp, nError, stHead);
        if(nError != 0)
        {
            nError = 6;
            break;
        }

        if(strcmp(stHead.szExt, ".zb") != 0)
        {
            nError = 7;
            break;
        }
        
        while(ifsp->peek() != EOF)
        {
            //读取zlib块的长度
            int nZlibBlockLen = 0;
            ifsp->read((char*)&nZlibBlockLen, sizeof(nZlibBlockLen));
            //读取zlib内容
            ifsp->read(szCompressed, nZlibBlockLen);
            std::streamsize readBytes = ifsp->gcount();
            szCompressed[readBytes] = '\0';
            std::cout << "Read bytes: " << readBytes << std::endl;
            //调用zlib进行解压缩
            int nUncompressLen = CCustomParamManager::Instance().GetBuffSize();
            int err = uncompress((Bytef*)szSource, 
                       (uLongf*)&nUncompressLen,
                       (const Bytef*)szCompressed,
                       (uLongf)nZlibBlockLen);
            if(err != 0)
            {
                nError = 8;
                break;
            }
            std::cout << "Uncompressed len: " << nUncompressLen << std::endl;
            ofsp->write(szSource, nUncompressLen);
        }

    }while(false);

    if(szSource) delete[] szSource;
    if(szCompressed) delete[] szCompressed;
    if(ifsp->is_open()) ifsp->close();
    if(ofsp->is_open()) ofsp->close();

    return nError;
}