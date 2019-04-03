#include "FileUtil.h"

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
    std::string strOut;
    std::string strInFile;
    int c;
    while ((c = getopt(argc, argv, "csou:hde?")) != -1)
    {
        switch (c)
        {
        case 'c':
            bCompress = true;
            strInFile = argv[optind];
            break;
        case 's':
            strInFile = argv[optind];
            break;
        case 'o':
            strOut = argv[optind];
            break;
        case 'u':
            strInFile = optarg;
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

    std::cout << "input_file:" << strInFile<< ",output_file:" << strOut << std::endl;

    if (bCompress)
    {
        CFileUtil::Compress(strInFile, strOut);
    }
    else
    {
        CFileUtil::Uncompress(strInFile, strOut);
    }
    
}
