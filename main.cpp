#include "FileUtil.h"
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
int main(int argc, char** argv)
{
    /*auto usage = [](std::ostream& os, const std::string& prog_name)
    {
        os << "Use: " << prog_name << " [-c] [-o output_file] files..." << std::endl
           << "Synposis:" << std::endl
           << "  Decompress (with `-c`, compress) files to stdout (with `-o`, to output_file)." << std::endl;
    };*/

    bool bCompress = false;
    int nEncode = -1;
    //默认根目录为当前目录
    std::string strOut;
    //int c;
    //默认CPU核数为1
    //默认使用1M

    po::options_description ops("command");
    po::variables_map vm;

    uint32_t dwCpuCore;
    uint64_t ullMaxBuffSize;
    std::vector<std::string> vecFiles;
    ops.add_options()
        ("compress_files,c", po::value<std::vector<std::string>>()->multitoken(), "files need to be compressed")
        ("uncompress_file,u", po::value<std::string>(), "files need to be uncompressed")
        ("file_need_to_be_encode,e", po::value<std::string>(), "file need to be encoded")
        ("file_need_to_be_decode,d", po::value<std::string>(), "file need to be decoded")
        ("Cpu_core,C", po::value<uint32_t>(&dwCpuCore)->default_value(1), "cpu cores")
        ("Buffer_size,B", po::value<uint64_t>(&ullMaxBuffSize)->default_value(1<<20), "max buffer size")
        ("out_put_file,o", po::value<std::string>(&strOut)->default_value(std::string(".")),"output files")
    ;
    po::store(po::parse_command_line(argc, argv, ops), vm);
    po::notify(vm);

    if (vm.count("compress_files")) 
    {
        bCompress = true;
        std::cout << "compress files: ";
        for (auto& v: vm["compress_files"].as<std::vector<std::string>>())
        {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }
    if (vm.count("uncompress_file")) 
    {
        std::string tmp = vm["uncompress_file"].as<std::string>();
        std::cout << "uncompress_file:" << tmp << std::endl;
    }
    if (vm.count("file_need_to_be_encode")) 
    {
        nEncode = 1;
        std::cout << "file_need_to_be_encode " << vm["file_need_to_be_encode"].as<std::string>() << std::endl;
    }
    if (vm.count("file_need_to_be_decode")) 
    {
        nEncode = 0;
        std::cout << "file_need_to_be_decode:" << vm["file_need_to_be_decode"].as<std::string>() << std::endl;
    }
    if (vm.count("Cpu_core")) 
    {
        std::cout << "Cpu_core:" << vm["Cpu_core"].as<uint32_t>() << std::endl;
    }
    if (vm.count("Buffer_size")) 
    {
        std::cout << "Buffer_size:" << vm["Buffer_size"].as<uint64_t>() << std::endl;
    }
    if (vm.count("out_put_file")) 
    {
        std::cout << "out_put_file:" << vm["out_put_file"].as<std::string>() << std::endl;
    }

    if(nEncode > -1)
    {
        if(nEncode)
        {
            CFileUtil::EncodeFile(vm["file_need_to_be_encode"].as<std::string>(), vm["out_put_file"].as<std::string>());
        }
        else
        {
            CFileUtil::DecodeFile(vm["file_need_to_be_decode"].as<std::string>(), vm["out_put_file"].as<std::string>());
        }
        return 0;
    }
    
    if (bCompress)
    {
        CFileUtil::Compress(vm["compress_files"].as<std::vector<std::string>>(),
                            vm["out_put_file"].as<std::string>(),
                            vm["Buffer_size"].as<uint64_t>(), 
                            vm["Cpu_core"].as<uint32_t>()
                            );
    }
    else
    {
        CFileUtil::Uncompress(vm["uncompress_file"].as<std::string>(),
                              vm["out_put_file"].as<std::string>(),
                              vm["Buffer_size"].as<uint64_t>(), 
                              vm["Cpu_core"].as<uint32_t>()
                              );
    }
}
