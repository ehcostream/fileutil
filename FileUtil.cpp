/*
 * FileUtil.cpp
 *
 *  Created on: 2019年4月3日
 *      Author: Edric
 *      Brief:  文件相关操作工具
 */
#include "FileUtil.h"


int CFileUtil::Compress(const std::string& rstrIn, const std::string& rstrOut)
{
    uint32_t dwNow = time(nullptr);
    //文件归档
    std::ostringstream ossTarTmp;
    ossTarTmp << "/tmp/zlibcompress_tmp_" << dwNow << ".tar";
    std::string strTarFile = ossTarTmp.str();
    std::ostringstream ossTarCmd;
    ossTarCmd << "tar" << " -cvf " << ossTarTmp.str() << " " << rstrIn;
    std::cout << ossTarCmd.str() << std::endl;
    FILE* fp = popen(ossTarCmd.str().c_str(), "w");
    pclose(fp);

    //调用zlib进行压缩
    std::unique_ptr< std::ostream > osp =
        (not rstrOut.empty()
         ? std::unique_ptr< std::ostream >(new zstr::ofstream(rstrOut))
         : std::unique_ptr< std::ostream >(new zstr::ostream(std::cout)));

    std::unique_ptr< std::ifstream > ifsp;
    std::istream * isp = &std::cin;

    ifsp = std::unique_ptr< std::ifstream >(new strict_fstream::ifstream(strTarFile));
    isp = ifsp.get();

    CatStream(*isp, *osp);

    //删除中间文件
    std::ostringstream ossdel;
    ossdel << "rm -rf " << strTarFile;
    FILE* fpd = popen(ossdel.str().c_str(), "w");
    pclose(fpd);

    return 0;
}

int CFileUtil::Uncompress(const std::string& rstrIn, const std::string& rstrOut)
{
    uint32_t dwNow = time(nullptr);
    //文件进行解压缩
    std::ostringstream ossTarTmp;
    ossTarTmp << "/tmp/zlibuncompress_tmp_" << dwNow << ".tar";
    std::string strUntarFile = ossTarTmp.str();
    std::unique_ptr< std::ofstream > ofsp;
    std::ostream * osp = &std::cout;
    if (not strUntarFile.empty())
    {
        ofsp = std::unique_ptr< std::ofstream >(new strict_fstream::ofstream(strUntarFile));
        osp = ofsp.get();
    }

    std::unique_ptr< std::istream > isp = std::unique_ptr< std::istream >(new zstr::ifstream(rstrIn));
    CatStream(*isp, *osp);

    //对归档文件释放到指定目录
    std::ostringstream ossUnTar;
    ossUnTar << "tar" << " -xvf " << strUntarFile << " -C " << rstrOut;
    FILE* fpuntar = popen(ossUnTar.str().c_str(), "w");
    pclose(fpuntar);

    //删除中间文件
    std::ostringstream ossdel;
    ossdel << "rm -rf " << strUntarFile;
    std::cout << ossdel.str() << std::endl;
    FILE* fpd = popen(ossdel.str().c_str(), "w");
    pclose(fpd);

    return 0;
}

int CFileUtil::EncodeFile(const std::string& rstrSource, const std::string& rstrEncodeFileDir)
{
    //加密后文件名称：源文件文件名+.spec后缀
    std::string strFileName;
    GetFileName(rstrSource, strFileName);
    std::ostringstream oss;
    oss << rstrEncodeFileDir << "/" << strFileName << ".spec";
    ReverseStream(rstrSource, oss.str());
    return 0;
}

int CFileUtil::DecodeFile(const std::string& rstrEncodeFile, const std::string& rstrDecodeFileDir)
{
    //默认解密为.tar.gz结尾的文件
    std::string strFileName;
    GetFileName(rstrEncodeFile, strFileName);
    std::ostringstream oss;
    oss << rstrDecodeFileDir << "/" << strFileName << ".tar.gz";
    ReverseStream(rstrEncodeFile, oss.str());
    return 0;
}

void CFileUtil::CatStream(std::istream& ris, std::ostream& ros)
{
    char * szBuff = new char [BUFF_SIZE];
    while (true)
    {
        ris.read(szBuff, BUFF_SIZE);
        std::streamsize cnt = ris.gcount();
        if (cnt == 0)
        {
            break;
        }
        ros.write(szBuff, cnt);
    }
    delete [] szBuff;
}

int CFileUtil::ReverseStream(const std::string& rstrSource, const std::string& rstrOut)
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
        std::cout << "before length :" << length << std::endl;
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

    std::cout << "after length :" << n << std::endl;

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

bool CFileUtil::GetFileName(const std::string& rstrFilePath, std::string& rstrFileName)
{
    std::vector<std::string> stFilePathPieces;
    boost::split(stFilePathPieces, rstrFilePath, boost::is_any_of("."));
    if(stFilePathPieces.size() > 0 && !stFilePathPieces[0].empty())
    {
        rstrFileName = stFilePathPieces[0];
        return true;
    }
    else
    {
        return false;
    }
}