#include "FileUtilBase.h"

void CFileUtilBase::SetSysParam(uint32_t dwCpuCore, uint64_t ullBufferSize)
{
    m_dwCpuCore = dwCpuCore;
    m_ullBuffSize = ullBufferSize;
}

bool CFileUtilBase::CatStream(std::istream& ris, std::ostream& ros)
{
    char* szBuff = new char[m_ullBuffSize];
    if(szBuff)
    {
        while (true)
        {
            ris.read(szBuff, m_ullBuffSize);
            std::streamsize cnt = ris.gcount();
            if (cnt == 0)
            {
                break;
            }
            ros.write(szBuff, cnt);
        }
        delete[] szBuff;
        return true;
    }
    else
    {
        return false;
    }
    
}

int CFileUtilBase::Archive(const std::vector<std::string>& rVecFile, const std::string& rstrOut)
{
    //遍历rVecFile
    //可能是单个文件，多个文件，文件夹，文件+文件夹（多级目录）
    //将文件头部信息和文件内容，序列化到单个文件中
    int nError = 0;
    do
    {
        std::ofstream oArcFile(rstrOut, std::ofstream::binary);
        //生成将要归档的文件
        std::cout << "out " << rstrOut << "open state " << oArcFile.is_open() << std::endl;
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
                    
                    if(fs::is_directory(path))
                    {
                        for(fs::recursive_directory_iterator dir_end, dir(rFile); dir != dir_end; ++dir)
                        {
                            std::cout << "--" << dir->path() << std::endl;
                            ArchiveOneFileOrDir(dir->path().string(), oArcFile);                            
                        }
                    }
                    else if(fs::is_regular_file(path))
                    {
                        std::cout << "is file: " << path.string() << std::endl;
                        ArchiveOneFileOrDir(path.string(), oArcFile); 
                    }
                    
                }
                catch(fs::filesystem_error e)
                {
                    std::cout << e.what() << std::endl;
                }
                
            }
        }
        oArcFile.close();
    }while(false);
    
    return nError;
}

int CFileUtilBase::Dearchive(const std::string& rstrArchivedFile, const std::string& rstrOut)
{
    //
    int nError = 0;
    assert(m_ullBuffSize != 0);

    do
    {
        try
        {
            std::ifstream iArcFile(rstrArchivedFile, std::ifstream::in | std::ifstream::binary);
            //输出路径如果存在，则直接以输出路径进行文件解档，如果不存在，则创建输出路径
            fs::path outPath(rstrOut);
            if(!fs::exists(outPath))
            {
                //不存在，创建此目录，并作为根目录
                fs::create_directories(outPath);
            }
            if(iArcFile.is_open())
            {
                //读取文件头信息
                while(iArcFile.peek() != EOF)
                {
                    //读取一个文件头+文件内容
                    //逐文件进行解档
                    DearchiveOneFileOrDir(iArcFile, rstrOut);
                }
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


int CFileUtilBase::ArchiveOneFileOrDir(const std::string& rstrSource, std::ofstream& rofArchiveFile)
{
    assert(m_ullBuffSize != 0);
    fs::path path(rstrSource);
    if(fs::is_regular_file(path))
    {
        std::ifstream iReadyFile(rstrSource, std::ifstream::binary);

        //该缓冲区会被反复写入，但都是根据指定字节长度读取或者写入，因此不用每次memset(pszBuff.get(), 0, m_ullBuffSize);
        std::unique_ptr<char> pszBuff = std::unique_ptr<char>(new char[m_ullBuffSize]);
        
        iReadyFile.seekg(0, iReadyFile.end);
        uint64_t length = iReadyFile.tellg();
        iReadyFile.seekg(0, iReadyFile.beg);

        //文件头部信息
        FileInfo fileInfo;
        fileInfo.bFType = FT_FILE;
        fileInfo.ullFSize = length;
        strncpy(fileInfo.szFPath, rstrSource.c_str(), rstrSource.length());
        strncpy(&fileInfo.szFPath[0] + rstrSource.length(), "\0", 1); 
        //写入头部信息
        rofArchiveFile.write((char*)&fileInfo,sizeof(FileInfo));

        char* szBuff = pszBuff.get();
        if(length > m_ullBuffSize)
        {
            //如果文件大小大于缓冲区大小
            std::cout << iReadyFile.peek() << EOF << std::endl;
            while(iReadyFile.peek() != EOF)
            {
                iReadyFile.read(szBuff, m_ullBuffSize);

                if(iReadyFile)
                {
                    //所有字符成功读取
                    rofArchiveFile.write(szBuff, m_ullBuffSize);
                }
                else
                {
                    //实际读取字节数
                    uint32_t readByteCnt = iReadyFile.gcount();
                    rofArchiveFile.write(szBuff, readByteCnt);
                }
            }
        }
        else
        {
            //如果文件大小小于缓冲区大小
            iReadyFile.read(szBuff, m_ullBuffSize);
            uint32_t dwSize = iReadyFile.gcount();
            rofArchiveFile.write(szBuff, dwSize);
        }
        
        iReadyFile.close();
    }
    else if(fs::is_directory(path))
    {
        FileInfo fileInfo;
        fileInfo.bFType = FT_DIR;
        fileInfo.ullFSize = 0;
        strncpy(fileInfo.szFPath, rstrSource.c_str(), rstrSource.length());
        strncpy(&fileInfo.szFPath[0] + rstrSource.length(), "\0", 1); 
        //写入头部信息
        rofArchiveFile.write((char*)&fileInfo,sizeof(FileInfo));
    }
    else
    {
        //DO NOT HANDLE
    }
    return 0;
}

int CFileUtilBase::DearchiveOneFileOrDir(std::ifstream& rifSource, const std::string& rstrOut)
{
    FileInfo stFileInfo;
    rifSource.read((char*)&stFileInfo, sizeof(FileInfo));


    char bType = stFileInfo.bFType;
    try
    {
        if(bType == FT_DIR)
        {
            fs::path dirPath = fs::path(rstrOut) / stFileInfo.szFPath;
            fs::create_directories(dirPath);
        }
        else if(bType == FT_FILE)
        {
            fs::path filePath = fs::path(rstrOut) / stFileInfo.szFPath;
            if(!fs::exists(filePath.parent_path()))
            {
                fs::create_directories(filePath.parent_path());
            }
            std::string strResultPath = filePath.string();
            std::cout << strResultPath << std::endl;

            std::ofstream oFile(strResultPath, std::ofstream::out | std::ofstream::binary);
            std::cout << strResultPath << ", File state deachive " << oFile.is_open() << std::endl;
            std::unique_ptr<char> pszBuff = std::unique_ptr<char>(new char[m_ullBuffSize]);
            char* szBuff = pszBuff.get();
            if(stFileInfo.ullFSize > m_ullBuffSize)
            {
                uint32_t dwNeedReadLoop = ceil(stFileInfo.ullFSize * 1.0 / m_ullBuffSize);
                uint32_t dwCnt = dwNeedReadLoop;
                std::cout << "loop:" << dwNeedReadLoop << std::endl;
                while(dwCnt--)
                {
                    std::cout << "current loop:" << dwCnt << std::endl;
                    if(dwCnt != 1)
                    {
                        rifSource.read(szBuff, m_ullBuffSize);
                        oFile.write(szBuff, m_ullBuffSize);
                    }
                    else
                    {
                        uint32_t dwRestBytes = stFileInfo.ullFSize - (dwNeedReadLoop - 1) * m_ullBuffSize;
                        std::cout << "rest:" << dwRestBytes << std::endl;
                        rifSource.read(szBuff, dwRestBytes);
                        std::cout << "read finished" << std::endl;
                        oFile.write(szBuff, dwRestBytes);
                        std::cout << "write finished" << std::endl;
                    }
                }
                
            }
            else
            {
                rifSource.read(szBuff, stFileInfo.ullFSize);
                oFile.write(szBuff, stFileInfo.ullFSize);
            }
            oFile.close();
        }
        else
        {
            //DO NOT HANDLE
        }
    }
    catch(fs::filesystem_error e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
    
}


void CFileUtilBase::GetTmpMiddleFile(std::string& rstrAchiveFile, bool bAchive, int nType)
{
    //当前微秒时间戳
    auto now = std::chrono::system_clock::now().time_since_epoch();
    int64_t ullNow = std::chrono::duration_cast<std::chrono::microseconds>(now).count();
    fs::path arFilePath = fs::temp_directory_path();
    std::ostringstream oss;
    if(nType > 0)
    {
        if (nType == 1)
        {
            oss << "zlib_mt_split_" << ullNow << ".ar";
        }
        if(nType == 2)
        {
            oss << "zlib_mt_result_" << ullNow << ".ar";
        }
    }
    else
    {
        oss << "zlib" << (bAchive ? "" : "un") << "compress_tmp_" << ullNow << ".ar";
    }
    
    arFilePath = arFilePath / oss.str();
    rstrAchiveFile = arFilePath.string();
}

int CFileUtilBase::CutFileIntoPieces(const std::string& rstrIn, std::vector<std::string>& rVecOutFiles, uint32_t dwBlcok)
{
    assert(dwBlcok > 0);
    std::cout << "CutFileIntoPieces：" << rstrIn << std::endl;
    std::ifstream inAchiFile(rstrIn);
    if(!inAchiFile.is_open())
    {
        return 1;
    }
    inAchiFile.seekg(0, inAchiFile.end);
    uint64_t ullTotalFileSize = inAchiFile.tellg();
    uint64_t ullSizeAvg = floor(ullTotalFileSize / dwBlcok);
    uint64_t ullLastPartSize = ullTotalFileSize - ullSizeAvg * (dwBlcok - 1);
    //恢复文件指针
    inAchiFile.seekg(0, inAchiFile.beg);
    for (uint32_t dwCurrent = 0; dwCurrent < dwBlcok && inAchiFile.peek() != EOF; ++dwCurrent)
    {
        std::string strOutFile;
        GetTmpMiddleFile(strOutFile, true, 1);
        std::ofstream outFile(strOutFile, std::ofstream::binary);
        char bReadByte;
        bool bLast = (dwCurrent == dwBlcok -1);

        for (uint64_t ullReadBytes = 0; ullReadBytes < (bLast ? ullLastPartSize : ullSizeAvg); ++ullReadBytes)
        {
            inAchiFile.read(&bReadByte, sizeof(char));
            outFile.write(&bReadByte, sizeof(char));
        }
        outFile.close();
        rVecOutFiles.emplace_back(strOutFile);
    }
    inAchiFile.close();
    return 0;
}

void CFileUtilBase::CombainFiles(const std::vector<std::string>& rVecInFiles, const std::string& rstrOutFile)
{
    std::ostream * osp = &std::cout;
    std::unique_ptr< std::ofstream > ofsp;
    ofsp = std::unique_ptr< std::ofstream >(new strict_fstream::ofstream(rstrOutFile));
    osp = ofsp.get();

    for(const auto& file : rVecInFiles)
    {
        std::cout << "combain file : " << file << std::endl;
        std::unique_ptr< std::istream > isp = std::unique_ptr< std::istream >(new strict_fstream::ifstream(file));
        CatStream(*isp, *osp);
    }
}