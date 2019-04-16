#include "FileUtil.h"
#include <unordered_map>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/any.hpp>
#include "zlibutil.h"

namespace po = boost::program_options;

typedef std::unordered_map<std::string, boost::any> PARAMMAP;
typedef std::unordered_map<std::string, std::vector<std::string>> PARAMLISTMAP;

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
OperationType GetCommandParam(int argc, char** argv, PARAMMAP& rParamMap)
{
        std::string strOut;
        uint32_t dwCpuCore;
        uint64_t ullMaxBuffSize;
        int nCompress = -1;
        int nEncode = -1;

        try
        {
            po::options_description ops("command");
            po::variables_map vm;
            ops.add_options()
                ("files_compress,c", po::value<std::vector<std::string>>()->multitoken(), "files need to be compressed")
                ("file_uncompress,u", po::value<std::string>(), "files need to be uncompressed")
                ("file_encode,e", po::value<std::string>(), "file need to be encoded")
                ("file_decode,d", po::value<std::string>(), "file need to be decoded")
                ("cpu_cores,C", po::value<uint32_t>(&dwCpuCore)->default_value(1), "cpu cores")
                ("max_buffer,B", po::value<uint64_t>(&ullMaxBuffSize)->default_value(1<<20), "max buffer size")
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
        }
        catch(...)
        {
            return OT_UNKNOW;
        }
        

        return GetOperResult(nCompress, nEncode);
}

int main(int argc, char** argv)
{
    //获取命令行参数
    PARAMMAP stParamMap;
    OperationType ot = GetCommandParam(argc, argv, stParamMap);

    std::cout << "in operation..." << std::endl;
    std::string strOutFile;
    std::string strEmptyFile;
    switch(ot)
    {
        case OT_COMPRESS:
        {
            std::vector<std::string> stFiles = boost::any_cast<std::vector<std::string>>(stParamMap[PKM_FC]);
            CFileUtil::Compress(boost::any_cast<std::vector<std::string>>(stParamMap[PKM_FC]), 
                                boost::any_cast<std::string>(stParamMap[PKM_OUT]), 
                                boost::any_cast<uint64_t>(stParamMap[PKM_MB]),
                                boost::any_cast<uint32_t>(stParamMap[PKM_CPU]),
                                strOutFile);
        }
        break;
        case OT_UNCOMPRESS:
        {
            CFileUtil::Uncompress(boost::any_cast<std::string>(stParamMap[PKM_FU]), 
                                  boost::any_cast<std::string>(stParamMap[PKM_OUT]), 
                                  boost::any_cast<uint64_t>(stParamMap[PKM_MB]), 
                                  boost::any_cast<uint32_t>(stParamMap[PKM_CPU]));
        }
        break;
        case OT_COMPRESS_WITH_ENCODE:
        {
            CFileUtil::Compress(boost::any_cast<std::vector<std::string>>(stParamMap[PKM_FC]), 
                                boost::any_cast<std::string>(stParamMap[PKM_OUT]), 
                                boost::any_cast<uint64_t>(stParamMap[PKM_MB]), 
                                boost::any_cast<uint32_t>(stParamMap[PKM_CPU]), 
                                strOutFile);
            CFileUtil::EncodeFile(strOutFile, boost::any_cast<std::string>(stParamMap[PKM_OUT]), strEmptyFile);
            fs::remove(strOutFile);
        }
        break;
        case OT_DECODE_WITH_UNCOMPRESS:
        {

            CFileUtil::DecodeFile(boost::any_cast<std::string>(stParamMap[PKM_FD]), 
                                  boost::any_cast<std::string>(stParamMap[PKM_OUT]), 
                                  strOutFile);
            CFileUtil::Uncompress(strOutFile, 
                                  boost::any_cast<std::string>(stParamMap[PKM_OUT]), 
                                  boost::any_cast<uint64_t>(stParamMap[PKM_MB]), 
                                  boost::any_cast<uint32_t>(stParamMap[PKM_CPU]));
            fs::remove(strOutFile);
        }
        break;
        case OT_ENCODE:
        {
            CFileUtil::EncodeFile(boost::any_cast<std::string>(stParamMap[PKM_FE]), 
                                  boost::any_cast<std::string>(stParamMap[PKM_OUT]), strEmptyFile);
        }
        break;
        case OT_DECODE:
        {
            CFileUtil::DecodeFile(boost::any_cast<std::string>(stParamMap[PKM_FD]), 
                                  boost::any_cast<std::string>(stParamMap[PKM_OUT]), 
                                  strOutFile);
        }
        break;
        case OT_UNKNOW:
        default:
            std::cout << "invalid arguments." << std::endl;
        break;
    }

    return 0;
}
