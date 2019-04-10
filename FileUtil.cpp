/*
 * FileUtil.cpp
 *
 *  Created on: 2019年4月3日
 *      Author: Edric
 *      Brief:  文件相关操作工具
 */
#include "FileUtil.h"

int CFileUtil::Compress(const std::vector<std::string>& rVecFile, const std::string& rstrOut, uint32_t dwBuffSize, uint32_t dwCpuCore)
{
    Archive(rVecFile, rstrOut, dwBuffSize);
    // uint32_t dwNow = time(nullptr);
    // //文件归档
    // std::ostringstream ossTarTmp;
    // ossTarTmp << "/tmp/zlibcompress_tmp_" << dwNow << ".tar";
    // std::string strTarFile = ossTarTmp.str();
    // std::ostringstream ossTarCmd;
    // ossTarCmd << "tar" << " -cvf " << ossTarTmp.str() << " " << rstrIn;
    // std::cout << ossTarCmd.str() << std::endl;
    // FILE* fp = popen(ossTarCmd.str().c_str(), "w");
    // pclose(fp);

    // //调用zlib进行压缩
    // std::unique_ptr< std::ostream > osp =
    //     (not rstrOut.empty()
    //      ? std::unique_ptr< std::ostream >(new zio::ofstream(rstrOut, dwBuffSize))
    //      : std::unique_ptr< std::ostream >(new zio::ostream(std::cout)));

    // std::unique_ptr< std::ifstream > ifsp;
    // std::istream * isp = &std::cin;

    // ifsp = std::unique_ptr< std::ifstream >(new strict_fstream::ifstream(strTarFile));
    // isp = ifsp.get();

    // CatStream(*isp, *osp, dwBuffSize);

    // //删除中间文件
    // std::ostringstream ossdel;
    // ossdel << "rm -rf " << strTarFile;
    // FILE* fpd = popen(ossdel.str().c_str(), "w");
    // pclose(fpd);

    return 0;
}

int CFileUtil::Uncompress(const std::string& rstrIn, const std::string& rstrOut, uint32_t dwBuffSize, uint32_t dwCpuCore)
{
    std::cout << dwBuffSize << std::endl;
    Dearchive(rstrIn, rstrOut, dwBuffSize);
    /*uint32_t dwNow = time(nullptr);
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

    std::unique_ptr< std::istream > isp = std::unique_ptr< std::istream >(new zio::ifstream(rstrIn, 1<<20));
    CatStream(*isp, *osp, dwBuffSize);

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
    pclose(fpd);*/

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

bool CFileUtil::CatStream(std::istream& ris, std::ostream& ros, uint32_t dwBuffSize)
{
    char* szBuff = new char[dwBuffSize];
    if(szBuff)
    {
        while (true)
        {
            ris.read(szBuff, dwBuffSize);
            std::streamsize cnt = ris.gcount();
            if (cnt == 0)
            {
                break;
            }
            ros.write(szBuff, cnt);
        }
        delete [] szBuff;
        return true;
    }
    else
    {
        return false;
    }
    
}

int CFileUtil::ReverseStream(const std::string& rstrSource, const std::string& rstrOut)
{
    std::cout << "source:" << rstrSource << "out:" << rstrOut << std::endl;
    std::ifstream in(rstrSource, std::ifstream::in | std::ifstream::binary);
    std::ofstream out(rstrOut, std::ofstream::out | std::ofstream::trunc);
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

int CFileUtil::Archive(const std::vector<std::string>& rVecFile, const std::string& rstrOut, uint32_t dwBuffSize)
{
    //遍历rVecFile
    //可能是单个文件，多个文件，文件夹，文件+文件夹（多级目录）
    //将文件头部信息和文件内容，序列化到单个文件中
    int nError = 0;
    std::vector<std::string> vecHierarchy;

    do
    {
        for(auto& rFile : rVecFile)
        {
            std::cout << rFile << std::endl;
            fs::path path(rFile);
            if(!fs::exists(path))
            {
                nError = -1;
                break;
            }
            else
            {
                try
                {
                    //生成将要归档的文件
                    std::ofstream oArcFile(rstrOut, std::ofstream::binary);
                    std::cout << "out " << rstrOut << "open state " << oArcFile.is_open() << std::endl;
                    if(fs::is_directory(path))
                    {
                        for(fs::recursive_directory_iterator dir_end, dir(rFile); dir != dir_end; ++dir)
                        {
                            std::cout << "--" << dir->path() << std::endl;
                            
                            if(fs::is_regular_file(dir->path()))
                            {
                                //判断是文件

                            }
                            else if(fs::is_directory(dir->path()))
                            {
                                //判断是文件夹
                                
                            }
                            else
                            {
                                //other files
                            }
                            
                        }
                    }
                    else if(fs::is_regular_file(path))
                    {
                        std::string strFileRelativePath = path.string();
                        std::ifstream iReadyFile(strFileRelativePath, std::ifstream::binary);

                        //该缓冲区会被反复写入，但都是根据指定字节读取或者写入，因此不用每次memset(pszBuff.get(), 0, dwBuffSize);
                        std::unique_ptr<char> pszBuff = std::unique_ptr<char>(new char[dwBuffSize]);
                        
                        iReadyFile.seekg(0, iReadyFile.end);
                        uint32_t length = iReadyFile.tellg();
                        iReadyFile.seekg(0, iReadyFile.beg);

                        //文件头部信息
                        FileInfo fileInfo;
                        fileInfo.dwFType = FT_FILE;
                        fileInfo.dwFSize = length;
                        strncpy(fileInfo.szFPath, strFileRelativePath.c_str(), strFileRelativePath.length());
                        //写入头部信息
                        oArcFile.write((char*)&fileInfo,sizeof(FileInfo));

                        char* szBuff = pszBuff.get();
                        if(length > dwBuffSize)
                        {
                            //如果文件大小大于缓冲区大小
                            std::cout << iReadyFile.peek() << EOF << std::endl;
                            while(iReadyFile.peek() != EOF)
                            {
                                iReadyFile.read(szBuff, dwBuffSize);

                                if(iReadyFile)
                                {
                                    //所有字符成功读取
                                    oArcFile.write(szBuff, dwBuffSize);
                                }
                                else
                                {
                                    //实际读取字节数
                                    uint32_t readByteCnt = iReadyFile.gcount();
                                    oArcFile.write(szBuff, readByteCnt);
                                }
                            }
                        }
                        else
                        {
                            //如果文件大小小于缓冲区大小
                            std::cout << "file length < buffer size" << std::endl;
                            iReadyFile.read(szBuff, dwBuffSize);
                            uint32_t dwSize = iReadyFile.gcount();
                            std::cout << "actual write bytes:" << dwSize << std::endl;
                            oArcFile.write(szBuff, dwSize);
                        }
                        
                        iReadyFile.close();
                        
                    }
                    std::cout << "close output file" << std::endl;
                    oArcFile.close();
                }
                catch(fs::filesystem_error e)
                {
                    std::cout << e.what() << std::endl;
                }
                
            }
        }
    }while(false);
    
    return nError;
}

int CFileUtil::Dearchive(const std::string& rstrArchivedFile, const std::string& rstrOut, uint32_t dwBuffSize)
{
    //
    int nError = 0;
    assert(dwBuffSize != 0);

    do
    {
        try
        {
            std::ifstream iArcFile(rstrArchivedFile, std::ifstream::in | std::ifstream::binary);

            if(iArcFile.is_open())
            {
                FileInfo stFileInfo;
                iArcFile.read((char*)&stFileInfo, sizeof(stFileInfo));
                //log file header
                std::cout << stFileInfo.dwFSize << " ,"
                << stFileInfo.dwFType << " ,"
                << stFileInfo.szFPath << std::endl;

                fs::path filePath = fs::path(rstrOut) / stFileInfo.szFPath;
                std::string strResultPath = filePath.string();
                std::cout << strResultPath << std::endl;

                std::ofstream oFile(strResultPath, std::ofstream::out | std::ofstream::binary);
                std::unique_ptr<char> pszBuff = std::unique_ptr<char>(new char[dwBuffSize]);
                char* szBuff = pszBuff.get();
                if(stFileInfo.dwFSize > dwBuffSize)
                {
                    uint32_t dwNeedReadLoop = ceil(stFileInfo.dwFSize * 1.0 / dwBuffSize);
                    uint32_t dwCnt = dwNeedReadLoop;
                    std::cout << "loop:" << dwNeedReadLoop << std::endl;
                    while(dwCnt--)
                    {
                        std::cout << "current loop:" << dwCnt << std::endl;
                        if(dwCnt != 1)
                        {
                            iArcFile.read(szBuff, dwBuffSize);
                            oFile.write(szBuff, dwBuffSize);
                        }
                        else
                        {
                            uint32_t dwRestBytes = stFileInfo.dwFSize - (dwNeedReadLoop - 1) * dwBuffSize;
                            std::cout << "rest:" << dwRestBytes << std::endl;
                            iArcFile.read(szBuff, dwRestBytes);
                            std::cout << "read finished" << std::endl;
                            oFile.write(szBuff, dwRestBytes);
                            std::cout << "write finished" << std::endl;
                        }
                    }
                    
                }
                else
                {
                    iArcFile.read(szBuff, stFileInfo.dwFSize);
                    oFile.write(szBuff, stFileInfo.dwFSize);
                }
                oFile.close();
                
            }
            else
            {
                //打开失败
                nError = 1;
                break;
            }
            iArcFile.close();
        }
        catch(...)
        {
            nError = -99;
            break;
        }
        
    }while(false);

    return nError;
}