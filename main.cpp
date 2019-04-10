#include "FileUtil.h"
#include <boost/lexical_cast.hpp>
int main(int argc, char** argv)
{
    auto usage = [](std::ostream& os, const std::string& prog_name)
    {
        os << "Use: " << prog_name << " [-c] [-o output_file] files..." << std::endl
           << "Synposis:" << std::endl
           << "  Decompress (with `-c`, compress) files to stdout (with `-o`, to output_file)." << std::endl;
    };


    bool bCompress = false;
    int nEncode = -1;
    //默认根目录为当前目录
    std::string strOut = ".";
    std::string strInFile;
    int c;
    //默认CPU核数为1
    int dwCpuCore = 1;
    //默认使用1M
    uint32_t dwMaxBuff = 1 << 20; 
    while ((c = getopt(argc, argv, "csouCB:hde?")) != -1)
    {
        switch (c)
        {
        case 'c':
            bCompress = true;
            //strInFile = argv[optind];
            break;
        case 's':
            strInFile = argv[optind];
            break;
        case 'o':
            strOut = argv[optind];
            break;
        case 'u':
            strInFile = argv[optind];
            break;
        case 'C':
            dwCpuCore = boost::lexical_cast<uint32_t>(argv[optind]);
            std::cout << dwCpuCore << std::endl;
            break;
        case 'B':
            dwMaxBuff = boost::lexical_cast<uint32_t>(argv[optind]);
            std::cout << dwMaxBuff << std::endl;
            break;
        case '?':
        case 'h':
            usage(std::cout, argv[0]);
            std::exit(EXIT_SUCCESS);
            break;
        case 'd':
            nEncode = 0;
            break;
        case 'e':
            nEncode = 1;
            break;
        default:
            usage(std::cerr, argv[0]);
            std::exit(EXIT_FAILURE);
        }
    }


    if(nEncode > -1)
    {
        if(nEncode)
        {
            CFileUtil::EncodeFile(strInFile, strOut);
        }
        else
        {
            CFileUtil::DecodeFile(strInFile, strOut);
        }
        return 0;
    }

    if (bCompress)
    {
        std::vector<std::string> vecFiles(&argv[optind+1], &argv[argc]);
        std::cout << "infiles:" << std::endl;
        for(const auto& it : vecFiles)
        {
            std::cout << it << std::endl;
        }
        std::cout << "out file:" << strOut << std::endl;
        
        CFileUtil::Compress(vecFiles, strOut, 1 << 20);
    }
    else
    {
        CFileUtil::Uncompress(strInFile, strOut, 1 << 20);
    }
    
}
