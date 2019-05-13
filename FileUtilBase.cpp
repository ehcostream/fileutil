#include "FileUtilBase.h"
#include "FileUtilHead.h"


bool CFileUtilBase::CatStream(std::istream& ris, std::ostream& ros)
{
    char* szBuff = new char[CCustomParamManager::Instance().GetBuffSize()];
    if(szBuff)
    {
        std::cout << ris.peek() << std::endl;
        while (true && ris.peek() != EOF)
        {
            ris.read(szBuff, CCustomParamManager::Instance().GetBuffSize());
            std::streamsize cnt = ris.gcount();
            if (cnt == 0)
            {
                std::cout << "break" << std::endl;
                break;
            }
            std::cout << "read " << cnt << std::endl;
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
    std::cout << __FUNCTION__ << "," << CCustomParamManager::Instance().GetBuffSize() << CCustomParamManager::Instance().GetCpuCore() << std::endl;
    int nError = 0;
    do
    {
        std::ofstream oArcFile(rstrOut, std::ofstream::binary);
        //生成将要归档的文件
        std::cout << "out " << rstrOut << ", open state " << oArcFile.is_open() << std::endl;
        for(const auto& rFile : rVecFile)
        {
            fs::path path(rFile);
            if(!fs::exists(path))
            {
                nError = 1;
                break;
            }
            else
            {
                try
                {
                    
                    if(fs::is_directory(path))
                    {
                        auto tmp = rFile;
                        if(std::string(&tmp.back()) == std::string("/") || std::string(&tmp.back()) == std::string("\\"))
                        {
                            tmp.pop_back();
                        }
                        std::string strRoot = fs::path(tmp).filename().string();
                        for(fs::recursive_directory_iterator dir_end, dir(rFile); dir != dir_end; ++dir)
                        {
                            nError = ArchiveOneFileOrDir(dir->path().string(), strRoot, oArcFile);
                        }
                        
                    }
                    else if(fs::is_regular_file(path))
                    {
                        std::cout << "File-->" << path.filename().string() << std::endl;

                        nError = ArchiveOneFileOrDir(path.string(), path.filename().string(), oArcFile); 
                    }
                    
                }
                catch(fs::filesystem_error e)
                {
                    std::cout << e.what() << std::endl;
                    nError = 99;
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
    assert(CCustomParamManager::Instance().GetBuffSize() != 0);

    do
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
        
        
    }while(false);

    return nError;
}


int CFileUtilBase::ArchiveOneFileOrDir(const std::string& rstrSource, const std::string& rstrRoot, std::ofstream& rofArchiveFile)
{
    try
    {
        assert(CCustomParamManager::Instance().GetBuffSize() != 0);
        fs::path path(rstrSource);
        //std::cout <<  ", " << path.parent_path() << "," << fs::canonical(fs::absolute(path)) << std::endl;
        if(fs::is_regular_file(path))
        {
            std::ifstream iReadyFile(rstrSource, std::ifstream::binary);
            //该缓冲区会被反复写入，但都是根据指定字节长度读取或者写入，因此不用每次memset(pszBuff.get(), 0, CCustomParamManager::Instance().GetBuffSize());
            std::unique_ptr<char> pszBuff = std::unique_ptr<char>(new char[CCustomParamManager::Instance().GetBuffSize()]);
            
            iReadyFile.seekg(0, iReadyFile.end);
            uint64_t length = iReadyFile.tellg();
            iReadyFile.seekg(0, iReadyFile.beg);

            //文件头部信息
            FileInfo fileInfo;
            memset(&fileInfo, '\0', sizeof(fileInfo));
            fileInfo.bFType = FT_FILE;
            fileInfo.ullFSize = length;

            std::string strSource = path.string();
            std::string strTarget;
            RebuildPath(strSource, rstrRoot, strTarget);
            strncpy(fileInfo.szFPath, strTarget.c_str(), strTarget.length());
            strncpy(&fileInfo.szFPath[0] + strTarget.length(), "\0", 1); 

            //写入头部信息
            rofArchiveFile.write((char*)&fileInfo,sizeof(FileInfo));

            char* szBuff = pszBuff.get();
            if(length > CCustomParamManager::Instance().GetBuffSize())
            {
                //如果文件大小大于缓冲区大小
                while(iReadyFile.peek() != EOF)
                {
                    iReadyFile.read(szBuff, CCustomParamManager::Instance().GetBuffSize());

                    if(iReadyFile)
                    {
                        //所有字符成功读取
                        rofArchiveFile.write(szBuff, CCustomParamManager::Instance().GetBuffSize());
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
                iReadyFile.read(szBuff, CCustomParamManager::Instance().GetBuffSize());
                uint32_t dwSize = iReadyFile.gcount();
                rofArchiveFile.write(szBuff, dwSize);
            }
            
            iReadyFile.close();
        }
        else if(fs::is_directory(path))
        {
            FileInfo fileInfo;
            memset(&fileInfo, '\0', sizeof(fileInfo));
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
    catch(...)
    {
        return 1;
    }
    
}

int CFileUtilBase::DearchiveOneFileOrDir(std::ifstream& rifSource, const std::string& rstrOut)
{
    FileInfo stFileInfo;
    memset(&stFileInfo, '\0', sizeof(stFileInfo));
    
    rifSource.read((char*)&stFileInfo, sizeof(FileInfo));
    
    char bType = stFileInfo.bFType;
    if(bType == FT_DIR)
    {
        fs::path dirPath = fs::path(rstrOut) / stFileInfo.szFPath;
        std::cout << "dir :" << dirPath.string() << std::endl;
        fs::create_directories(dirPath);
    }
    else if(bType == FT_FILE)
    {
    
        fs::path filePath = (fs::path(rstrOut) / stFileInfo.szFPath);
        std::cout << filePath.string() << std::endl;
        std::cout << filePath.parent_path().string() << std::endl;;

        if(!fs::exists( filePath.parent_path() ))
        {
            std::cout << filePath.parent_path().string() << std::endl;
            std::cout << "not exist," << filePath.parent_path().string() << std::endl;;
            fs::create_directories(filePath.parent_path());
        }
        
        std::string strResultPath = filePath.string();

        std::ofstream oFile(strResultPath, std::ofstream::out | std::ofstream::binary);
        
        if(stFileInfo.ullFSize != 0)
        {

            if(stFileInfo.ullFSize > CCustomParamManager::Instance().GetBuffSize())
            {
                std::unique_ptr<char> pszBuff = std::unique_ptr<char>(new char[CCustomParamManager::Instance().GetBuffSize()]);
                char* szBuff = pszBuff.get();
                uint32_t dwNeedReadLoop = ceil(stFileInfo.ullFSize * 1.0 / CCustomParamManager::Instance().GetBuffSize());

                uint32_t dwCnt = dwNeedReadLoop;
                std::cout << "loop:" << dwNeedReadLoop << std::endl;
                while(dwCnt--)
                {
                    std::cout << "current loop:" << dwCnt << std::endl;
                    if(dwCnt != 1)
                    {
                        rifSource.read(szBuff, CCustomParamManager::Instance().GetBuffSize());
                        oFile.write(szBuff, CCustomParamManager::Instance().GetBuffSize());
                    }
                    else
                    {
                        uint32_t dwRestBytes = stFileInfo.ullFSize - (dwNeedReadLoop - 1) * CCustomParamManager::Instance().GetBuffSize();
                        std::cout << "rest:" << dwRestBytes << std::endl;
                        rifSource.read(szBuff, dwRestBytes);
                        oFile.write(szBuff, dwRestBytes);
                    }
                }
            }
            else
            {
                std::unique_ptr<char> pszBuff = std::unique_ptr<char>(new char[stFileInfo.ullFSize]);
                char* szBuff = pszBuff.get();
                std::cout << "file size :" << stFileInfo.ullFSize << std::endl;
                rifSource.read(szBuff, stFileInfo.ullFSize);
                oFile.write(szBuff, stFileInfo.ullFSize);
            }
        }
        else
        {
            std::cout << "File size 0" << std::endl;
        }
        oFile.close();
    }
    else
    {
        //DO NOT HANDLE
    }
    return 0;
}


void CFileUtilBase::GetTmpMiddleFile(std::string& rstrAchiveFile, bool bAchive, int nType, int nThreadID)
{
    //当前微秒时间戳
    auto now = std::chrono::system_clock::now().time_since_epoch();
    int64_t ullNow = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
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
            if(nThreadID > 0)
            {
                oss << "zlib_mt_result_" << ullNow << "_" << nThreadID << ".ar";
            }
            else
            {
                oss << "zlib_mt_result_" << ullNow << ".ar";
            }
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
    assert(dwBlcok > 1);
    std::cout << "CutFile->" << rstrIn << std::endl;
    std::ifstream inAchiFile(rstrIn, std::ifstream::binary);
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
    std::cout << ullTotalFileSize << std::endl;
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
    std::unique_ptr< std::ofstream > osp = std::unique_ptr< std::ofstream >(new strict_fstream::ofstream(rstrOutFile));
    //附加头信息
    CFileUtilHead::Attach(*osp, rstrOutFile);
    for(const auto& file : rVecInFiles)
    {
        std::cout << "combain file : " << file << std::endl;
        std::unique_ptr< std::ifstream > isp = std::unique_ptr< std::ifstream >(new strict_fstream::ifstream(file));
        CatStream(*isp, *osp);
        isp->close();
    }
    osp->close();
}

void CFileUtilBase::RebuildPath(const std::string& rstrSource, const std::string& rstrRoot, std::string& rstrTarget)
{
    rstrTarget = rstrSource;
    size_t pos = rstrTarget.find(rstrRoot);
    if(pos != std::string::npos)
    {
        if(pos > 1)
        {
            rstrTarget.erase(0, pos - 1);
            rstrTarget = (fs::path(".") / rstrTarget).string();
        }
        else
        {
            std::cout << "file path is not invalid" << std::endl;
        }
    }
    else
    {
        std::cout << "'" << rstrTarget << "' is not in the string '" << rstrSource << "'" << std::endl;
    }
}