#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdio.h>
#include <ostream>
#include <unistd.h>
#include <time.h>
#include "zstr.cpp"

void usage(std::ostream& os, const std::string& prog_name)
{
    os << "Use: " << prog_name << " [-c] [-o output_file] files..." << std::endl
       << "Synposis:" << std::endl
       << "  Decompress (with `-c`, compress) files to stdout (with `-o`, to output_file)." << std::endl;
}

void cat_stream(std::istream& is, std::ostream& os)
{
    const std::streamsize buff_size = 1 << 16;
    char * buff = new char [buff_size];
    while (true)
    {
        is.read(buff, buff_size);
        std::streamsize cnt = is.gcount();
        if (cnt == 0) break;
        os.write(buff, cnt);
    }
    delete [] buff;
}

void Compress(const std::string& rstrIn, const std::string& rstrOut)
{
    std::unique_ptr< std::ostream > osp =
        (not rstrOut.empty()
         ? std::unique_ptr< std::ostream >(new zstr::ofstream(rstrOut))
         : std::unique_ptr< std::ostream >(new zstr::ostream(std::cout)));

    std::unique_ptr< std::ifstream > ifsp;
    std::istream * isp = &std::cin;

    ifsp = std::unique_ptr< std::ifstream >(new strict_fstream::ifstream(rstrIn));
    isp = ifsp.get();

    cat_stream(*isp, *osp);
}

void Uncompress(const std::string& rstrIn, const std::string& rstrOut)
{
    std::unique_ptr< std::ofstream > ofsp;
    std::ostream * osp = &std::cout;
    if (not rstrOut.empty())
    {
        ofsp = std::unique_ptr< std::ofstream >(new strict_fstream::ofstream(rstrOut));
        osp = ofsp.get();
    }

    std::unique_ptr< std::istream > isp = std::unique_ptr< std::istream >(new zstr::ifstream(rstrIn));

    cat_stream(*isp, *osp);
}

int ReverseFile(const std::string& rstrSource, const std::string& rstrOut)
{
    std::cout << "source:" << rstrSource << "out:" << rstrOut << std::endl;
    std::ifstream in(rstrSource, std::ifstream::in | std::ifstream::binary);
    std::ofstream out(rstrOut, std::ifstream::out | std::ifstream::trunc);
    int nError = 0;
    int n = 0;
    do
    {
        if(!in.is_open())
        {
            nError = 1;
            break;
        }
        if(!out.is_open())
        {
            nError = 2;
            break;
        }
        //open file normally
        char c;
        in.seekg (0, in.end);
        int length = in.tellg();
        in.seekg (0, in.beg);
        std::cout << "f length :" << length << std::endl;
        while(n < length)
        {
            in.get(c);
            char tmp = ~c;
            out << tmp;
            n++;
        }
        in.close();
        out.close();

    }while(false);

    std::cout << "file length :" << n << std::endl;

    if(nError == 0 )
    {
        std::cout << "encode file successfully" << std::endl;
    }
    else
    {
        std::cout << "encode file failed." << std::endl;
    }
    return nError;
}

int main(int argc, char * argv[])
{
    bool bCompress = false;
    int nEncode = -1;
    std::string strOutFile;
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
            std::cout << strInFile << std::endl;
            break;
        case 'o':
            strOutFile = argv[optind];
            break;
        case 'u':
            strInFile = argv[optind];
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
            ReverseFile(strInFile, std::string("./test.spec"));
        }
        else
        {
            ReverseFile(strInFile, std::string("./test.tar.gz"));
        }
        
        return 0;
    }

    uint32_t dwNow = time(nullptr);    

    std::cout << "output_file:" << strOutFile << std::endl;

    if (bCompress)
    {
        std::ostringstream ossTmp;
        ossTmp << "/tmp/zlibcompress_tmp_" << dwNow << ".tar";
        std::string strTarFile = ossTmp.str();
        std::ostringstream oss;
        oss << "tar" << " -cvf " << ossTmp.str() << " " << strInFile;
        std::cout << oss.str() << std::endl;

        FILE* fp = popen(oss.str().c_str(), "w");
        pclose(fp);
        Compress(strTarFile, strOutFile);

        std::ostringstream ossdel;
        ossdel << "rm -rf " << strTarFile;
        FILE* fpd = popen(ossdel.str().c_str(), "w");
        pclose(fpd);
    }
    else
    {
        
        //decompress_files(strInFile, strOutFiles);
        std::ostringstream ossTmp;
        ossTmp << "/tmp/zlibuncompress_tmp_" << dwNow << ".tar";
        std::string strUntarFile = ossTmp.str();

        Uncompress(strInFile, strUntarFile);
        std::ostringstream ossUnTar;
        ossUnTar << "tar" << " -xvf " << strUntarFile << " -C " << strOutFile;
        FILE* fpuntar = popen(ossUnTar.str().c_str(), "w");
        pclose(fpuntar);

        std::ostringstream ossdel;
        ossdel << "rm -rf " << strUntarFile;
        std::cout << ossdel.str() << std::endl;
        FILE* fpd = popen(ossdel.str().c_str(), "w");
        pclose(fpd);
    }
    
}
