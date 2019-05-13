#include "FileUtilGenerator.h"
#include "FileUtilGeneratorAsync.h"
#include "ThreadPool.h"
#include "VersionInfo.h"
#include <chrono>
#include <thread>
namespace po = boost::program_options;

typedef std::unordered_map<std::string, boost::any> PARAMMAP;

enum OperationType
{
    OT_UNKNOW = 0,
    OT_COMPRESS = 1,
    OT_UNCOMPRESS = 2,
    OT_ENCODE = 3,
    OT_DECODE = 4,
    OT_DECODE_WITH_UNCOMPRESS = 5,
    OT_COMPRESS_WITH_ENCODE = 6,
    OT_MAX
};

static const std::string PKM_FC = "files_compress";
static const std::string PKM_FU = "file_uncompress";
static const std::string PKM_FE = "file_encode";
static const std::string PKM_FD = "file_decode";
static const std::string PKM_CPU = "cpu_cores";
static const std::string PKM_MB = "max_buffer";
static const std::string PKM_OUT = "out_file";
static const std::string PKM_KEY = "encode_key";
static const std::string PKM_ASYNC = "async";

//参数检查&获取当前操作类型
OperationType GetOperResult(int nCompress, int nEncode)
{
    OperationType op = OT_UNKNOW;
    if(nCompress >= 0 && nEncode >= 0)
    {
        if(nCompress == 1 && nEncode == 1)
        {
            op = OT_COMPRESS_WITH_ENCODE;
        }
        else if(nCompress == 0 && nEncode == 0)
        {
            op = OT_DECODE_WITH_UNCOMPRESS;
        }
        else
        {
            //invalid
        }
    }
    else if(nCompress >= 0)
    {
        //只压缩/解压
        if(nCompress == 0)
        {
            op = OT_UNCOMPRESS;
        }
        else if(nCompress == 1)
        {
            op = OT_COMPRESS;
        }
        else
        {
            //invalid
        }
    }
    else if(nEncode >= 0)
    {
        //只加密/解密
        if(nEncode == 1)
        {
            op = OT_ENCODE;
        }
        else if(nEncode == 0)
        {
            op = OT_DECODE;
        }
        else
        {
            //invalid
        }
    }
    else
    {
        //invalid
    }
    return op;
};

//命令行参数转换map
OperationType GetCommandParam(int argc, char** argv, PARAMMAP& rParamMap, bool& rnAsync)
{
        std::string strOut;
        uint32_t dwCpuCore;
        uint64_t ullMaxBuffSize;
        int nCompress = -1;
        int nEncode = -1;
        rnAsync = false;
        try
        {
            po::options_description ops("command");
            po::variables_map vm;
            ops.add_options()
                ("async,ay", "non block mode")
                ("files_compress,c", po::value<std::vector<std::string>>()->multitoken(), "files need to be compressed")
                ("file_uncompress,u", po::value<std::string>(), "files need to be uncompressed")
                ("file_encode,e", po::value<std::string>(), "file need to be encoded")
                ("file_decode,d", po::value<std::string>(), "file need to be decoded")
                ("cpu_cores,C", po::value<uint32_t>(&dwCpuCore)->default_value(1), "cpu cores")
                ("max_buffer,B", po::value<uint64_t>(&ullMaxBuffSize)->default_value(1<<20), "max buffer size")
                ("encode_key,k", po::value<std::string>(), "key for encoding")
                ("out_file,o", po::value<std::string>(&strOut)->default_value(std::string(".")),"output files")
            ;
            po::store(po::parse_command_line(argc, argv, ops), vm);

            po::notify(vm);
            
            if (vm.count(PKM_FC)) 
            {
                nCompress = 1;
                std::vector<std::string> stFiles;
                for (auto& v: vm[PKM_FC].as<std::vector<std::string>>())
                {
                    stFiles.emplace_back(v);
                }
                rParamMap[PKM_FC] = stFiles;
            }
            if (vm.count(PKM_FU)) 
            {
                nCompress = 0;
                rParamMap[PKM_FU] = vm[PKM_FU].as<std::string>();
            }
            if (vm.count(PKM_FE)) 
            {
                nEncode = 1;
                rParamMap[PKM_FE] = vm[PKM_FE].as<std::string>();
            }
            if (vm.count(PKM_FD)) 
            {
                nEncode = 0;
                rParamMap[PKM_FD] = vm[PKM_FD].as<std::string>();
            }
            if (vm.count(PKM_CPU)) 
            {
                rParamMap[PKM_CPU] = vm[PKM_CPU].as<uint32_t>();
            }
            if (vm.count(PKM_MB)) 
            {
                rParamMap[PKM_MB] = vm[PKM_MB].as<uint64_t>();
            }
            if (vm.count(PKM_OUT)) 
            {
                rParamMap[PKM_OUT] = vm[PKM_OUT].as<std::string>();
            }
            if(vm.count(PKM_KEY))
            {
                rParamMap[PKM_KEY] = vm[PKM_KEY].as<std::string>();
            }
            else
            {
                if(nEncode > -1)
                {
                    return OT_UNKNOW;
                }
            }
            if (vm.count(PKM_ASYNC)) 
            {
                rnAsync = true;
            }
        }
        catch(...)
        {
            return OT_UNKNOW;
        }
        

        return GetOperResult(nCompress, nEncode);
}

int main(int argc, char** argv)
{
    std::string strEmptyFile;
    std::string strOutFile;
    std::cout << "Main thread" << "\t" << boost::this_thread::get_id() << std::endl;

    //获取命令行参数
    PARAMMAP stParamMap;
    bool bAsync = false;
    OperationType ot = GetCommandParam(argc, argv, stParamMap, bAsync);

    CFileUtilGeneratorBase* pstBase = nullptr;
    std::cout << "Mode:" << bAsync << std::endl;
    std::cout << "Version:" << CVersionInfo::String() << std::endl;
    if(bAsync)
    {
        pstBase = &CFileUtilGeneratorAsync::Instance();
    }
    else
    {
        pstBase = &CFileUtilGenerator::Instance();
    }

    //生成相应的工具
    //为生成器设置行动参数
    pstBase->Set(boost::any_cast<uint32_t>(stParamMap[PKM_CPU]), boost::any_cast<uint64_t>(stParamMap[PKM_MB]));
    
    std::cout << "Processing ..." << std::endl;
    int nError = 0;
    
    switch(ot)
    {
        case OT_COMPRESS:
        {
            std::unique_ptr<CFileUtilBase> pstCompresser = std::unique_ptr<CFileUtilBase>(pstBase->CreateCompresser());
            if(pstCompresser == nullptr)
            {
                nError = 1;
                break;
            }
            nError = pstCompresser->Execute(boost::any_cast<std::vector<std::string>>(stParamMap[PKM_FC]), 
                                   boost::any_cast<std::string>(stParamMap[PKM_OUT]), 
                                   nullptr, 
                                   strOutFile);
        }
        break;
        case OT_UNCOMPRESS:
        {
            std::unique_ptr<CFileUtilBase> pstUncompresser = std::unique_ptr<CFileUtilBase>(pstBase->CreateUncompresser(boost::any_cast<std::string>(stParamMap[PKM_FU])));
            if(pstUncompresser == nullptr)
            {
                nError = 2;
                break;
            }
            std::vector<std::string> stvecFiles;
            stvecFiles.emplace_back(boost::any_cast<std::string>(stParamMap[PKM_FU]));
            nError = pstUncompresser->Execute(stvecFiles,
                                     boost::any_cast<std::string>(stParamMap[PKM_OUT]),
                                     nullptr,
                                     strOutFile);
        }
        break;
        case OT_COMPRESS_WITH_ENCODE:
        {
            if(!bAsync)
            {
                std::unique_ptr<CFileUtilBase> pstCompresser = std::unique_ptr<CFileUtilBase>(pstBase->CreateCompresser());
                std::unique_ptr<CFileUtilBase> pstEncoder = std::unique_ptr<CFileUtilBase>(pstBase->CreateEncoder());
                nError = pstCompresser->Execute(boost::any_cast<std::vector<std::string>>(stParamMap[PKM_FC]), 
                                   boost::any_cast<std::string>(stParamMap[PKM_OUT]), 
                                   nullptr, 
                                   strOutFile);
                std::string strTmpFile = strOutFile;
                std::vector<std::string> stvecFiles;
                stvecFiles.emplace_back(strOutFile);
                nError = pstEncoder->Execute(stvecFiles, 
                                    boost::any_cast<std::string>(stParamMap[PKM_OUT]), 
                                    (char*)boost::any_cast<std::string>(stParamMap[PKM_KEY]).c_str(), 
                                    strOutFile);
                fs::remove(strTmpFile);
            }
            else
            {
                std::cout << "temporary unsupported" << std::endl;
            }
        }
        break;
        case OT_DECODE_WITH_UNCOMPRESS:
        {
            if(!bAsync)
            {
                std::unique_ptr<CFileUtilBase> pstDecoder = std::unique_ptr<CFileUtilBase>(pstBase->CreateDecoder(boost::any_cast<std::string>(stParamMap[PKM_FD])));
                if(pstDecoder == nullptr)
                {
                    nError = 3;
                    break;
                }
                std::vector<std::string> stvecFiles;
                stvecFiles.emplace_back(boost::any_cast<std::string>(stParamMap[PKM_FD]));
                int nError = pstDecoder->Execute(stvecFiles, 
                                    boost::any_cast<std::string>(stParamMap[PKM_OUT]), 
                                    (char*)boost::any_cast<std::string>(stParamMap[PKM_KEY]).c_str(), 
                                    strOutFile);
                if(nError != 0)
                {
                    std::cout << "decode failed, error code: " << nError << std::endl;
                    return 0 ;
                }
                std::unique_ptr<CFileUtilBase> pstUncompresser = std::unique_ptr<CFileUtilBase>(pstBase->CreateUncompresser(strOutFile));

                std::string strTmpFile = strOutFile;
                stvecFiles.clear();
                stvecFiles.emplace_back(strOutFile);
                pstUncompresser->Execute(stvecFiles, 
                                    boost::any_cast<std::string>(stParamMap[PKM_OUT]), 
                                    nullptr, 
                                    strOutFile);
                fs::remove(strTmpFile);
            }
            else
            {
                std::cout << "temporary unsupported" << std::endl;
            }
        }
        break;
        case OT_ENCODE:
        {
            std::unique_ptr<CFileUtilBase> pstEncoder = std::unique_ptr<CFileUtilBase>(pstBase->CreateEncoder());
            std::vector<std::string> stvecFiles;
            std::cout << boost::any_cast<std::string>(stParamMap[PKM_KEY]) << std::endl;
            stvecFiles.emplace_back(boost::any_cast<std::string>(stParamMap[PKM_FE]));
            nError = pstEncoder->Execute(stvecFiles, 
                                boost::any_cast<std::string>(stParamMap[PKM_OUT]), 
                                (char*)boost::any_cast<std::string>(stParamMap[PKM_KEY]).c_str(), 
                                strOutFile);
        }
        break;
        case OT_DECODE:
        {
            std::unique_ptr<CFileUtilBase> pstDecoder = std::unique_ptr<CFileUtilBase>(pstBase->CreateDecoder(boost::any_cast<std::string>(stParamMap[PKM_FD])));
            if(pstDecoder == nullptr)
            {
                nError = 4;
                break;
            }
            std::vector<std::string> stvecFiles;
            std::cout << boost::any_cast<std::string>(stParamMap[PKM_KEY]) << std::endl;
            stvecFiles.emplace_back(boost::any_cast<std::string>(stParamMap[PKM_FD]));
            int nError = pstDecoder->Execute(stvecFiles, 
                                boost::any_cast<std::string>(stParamMap[PKM_OUT]), 
                                (char*)boost::any_cast<std::string>(stParamMap[PKM_KEY]).c_str(), 
                                strOutFile);
            if(nError != 0)
            {
                std::cout << "decode failed, error code: " << nError << std::endl;
                return 0 ;
            }
        }
        break;
        case OT_UNKNOW:
        default:
            std::cout << "invalid arguments." << std::endl;
            nError = 99;
        break;
    }
    CThreadPool::Instance().JoinAll();

    if(nError != 0)
    {
        std::cout << "Operation failed, error code: " << nError << std::endl;
    }
    else
    {
        std::cout << "Operation finished" << std::endl;    
    }
    return 0;
}
