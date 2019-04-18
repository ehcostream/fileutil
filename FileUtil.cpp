/*
 * FileUtil.cpp
 *
 *  Created on: 2019年4月3日
 *      Author: Edric
 *      Brief:  文件相关操作工具
 */
#include "FileUtil.h"

void CFileUtil::GetTmpMiddleFile(std::string& rstrAchiveFile, bool bAchive, int nMT)
{
    //当前微秒时间戳
    auto now = std::chrono::system_clock::now().time_since_epoch();
    int64_t ullNow = std::chrono::duration_cast<std::chrono::microseconds>(now).count();
    fs::path arFilePath = fs::temp_directory_path();
    std::ostringstream oss;
    if(nMT > 0)
    {
        if (nMT == 1)
        {
            oss << "zlib_mt_split_" << ullNow << ".ar";
        }
        if(nMT == 2)
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

int CFileUtil::Compress(const std::vector<std::string>& rVecFile, const std::string& rstrOut, uint32_t dwBuffSize, uint32_t dwCpuCore, std::string& rstrOutFile)
{
    std::cout << "Compress" << std::endl;
    assert(rVecFile.size() > 0);
    //归档
    std::string strMidFile;
    GetTmpMiddleFile(strMidFile, true);
    assert(!strMidFile.empty());
    Archive(rVecFile, strMidFile, dwBuffSize);

    fs::path outDir(rstrOut);
    if(!fs::exists(outDir))
    {
        fs::create_directories(outDir);
    }

    fs::path outFilePath(outDir);
    std::string strExtension = ".ar.zb";
    std::string strTargetFile;

    auto file = rVecFile.front();
    if(rVecFile.size() > 1)
    {
        //如果有多个文件/夹，以第一个文件的父目录名称为文件名建立压缩文件
        strTargetFile = fs::path(fs::absolute(file)).parent_path().filename().string();
    }
    else
    {
        fs::path path(file);
        if(fs::is_regular_file(path))
        {
            strTargetFile = path.stem().string();
        }
        if(fs::is_directory(path))
        {
            if(std::string(&file.back()) == std::string("/") || std::string(&file.back()) == std::string("\\"))
            {
                file.pop_back();
            }
            path = fs::path(file);
            strTargetFile = path.filename().string();
        }
        
        std::cout << strTargetFile << std::endl;
    }
    strTargetFile.append(strExtension);
    outFilePath /= strTargetFile;
    std::cout << "compress out file :" << outFilePath << std::endl;
    rstrOutFile = outFilePath.string();
    if(dwCpuCore > 1)
    {
        //多线程压缩
        CompressWithMT(strMidFile, dwBuffSize, dwCpuCore, outFilePath.string());
    }
    else
    {
        //调用zlib进行压缩
        std::unique_ptr< std::ostream > osp =
            (not rstrOut.empty()
             ? std::unique_ptr< std::ostream >(new zio::ofstream(outFilePath.string(), dwBuffSize))
             : std::unique_ptr< std::ostream >(new zio::ostream(std::cout)));

        std::unique_ptr< std::ifstream > ifsp;
        std::istream * isp = &std::cin;

        ifsp = std::unique_ptr< std::ifstream >(new strict_fstream::ifstream(strMidFile));
        isp = ifsp.get();
        CatStream(*isp, *osp, dwBuffSize);

        //删除临时文件
        fs::remove(fs::path(strMidFile));
    }

    
    return 0;
}

int CFileUtil::CompressWithMT(const std::string rstrAchiveFile, uint32_t dwBuffSize, uint32_t dwCpuCore, const std::string& rstrOut)
{
    //切割为多个多文件
    //开启线程池，加入多个线程，根据buffsize平均分配，同时压缩文件
    //等待所有线程完成，将结果写入目标文件中
    assert(dwBuffSize > 0 && dwCpuCore > 1);

    uint32_t dwThreadCnt = dwCpuCore;
    std::vector<std::string> vecFiles;
    CutFileIntoPieces(rstrAchiveFile, vecFiles, dwThreadCnt);
    if(vecFiles.size() != dwThreadCnt)
    {
        std::cout << "vecfiles size:" << vecFiles.size() << std::endl;;
        return 1;
    }

    std::vector<ThreadParam> vecThreadList;
    uint32_t dwGeneralAvg = floor(dwBuffSize / dwThreadCnt);
    boost::thread_group threadPool;
    //初始化线程池
    for(uint32_t i = 0; i< dwThreadCnt; i++)
    {
        ThreadParam stThreadParam;
        if(i == dwThreadCnt -1)
        {
            stThreadParam.dwBuffSize = dwBuffSize - dwGeneralAvg * (dwThreadCnt - 1);
        }
        else
        {
            stThreadParam.dwBuffSize = dwGeneralAvg;
        }
        stThreadParam.strSource = vecFiles[i];
        vecThreadList.emplace_back(stThreadParam);
    }

    std::cout << "thread prepare..." << vecThreadList.size() << std::endl;
    //多线程同时压缩数据
    for(auto& param : vecThreadList)
    {
        threadPool.create_thread(boost::bind(CompressAFile, &param));
    }

    threadPool.join_all();    
    
    std::cout << "thread finished" << std::endl;
    //处理完成重组数据
    std::vector<std::string> vecResultFiles;
    for(const auto& param : vecThreadList)
    {
        vecResultFiles.emplace_back(param.strOutFile);
    }

    std::cout << "ready to combain files" << std::endl;
    //重组数据
    CombainFiles(vecResultFiles, rstrOut, dwBuffSize);

    //清理中间文件
    fs::remove(fs::path(rstrAchiveFile));
    for(const auto& file : vecFiles)
    {
        fs::remove(fs::path(file));
    }
    for(const auto& file : vecResultFiles)
    {
        fs::remove(fs::path(file));
    }

    return 0;
}

void CFileUtil::CompressAFile(void* pParam)
{
    assert(pParam != nullptr);
    ThreadParam* pTParam = (ThreadParam*)pParam;
    std::cout << pTParam->strOutFile << pTParam->strSource << pTParam->dwBuffSize << std::endl;
    GetTmpMiddleFile(pTParam->strOutFile, true, 2);
    std::cout << "  thread source--->" << pTParam->strSource << std::endl;
    std::cout << "  thread result--->" << pTParam->strOutFile << std::endl;
    std::unique_ptr< std::ostream > osp = std::unique_ptr< std::ostream >(new zio::ofstream(pTParam->strOutFile, pTParam->dwBuffSize));
    std::unique_ptr< std::ifstream > ifsp = std::unique_ptr< std::ifstream >(new strict_fstream::ifstream(pTParam->strSource));
    std::istream * isp = ifsp.get();
    CatStream(*isp, *osp, pTParam->dwBuffSize);
}

int CFileUtil::CutFileIntoPieces(const std::string& rstrIn, std::vector<std::string>& rVecOutFiles, uint32_t dwBlcok)
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

void CFileUtil::CombainFiles(const std::vector<std::string>& rVecInFiles, const std::string& rstrOutFile, uint32_t dwBuffSize)
{
    std::ostream * osp = &std::cout;
    std::unique_ptr< std::ofstream > ofsp;
    ofsp = std::unique_ptr< std::ofstream >(new strict_fstream::ofstream(rstrOutFile));
    osp = ofsp.get();

    for(const auto& file : rVecInFiles)
    {
        std::cout << "combain file : " << file << std::endl;
        std::unique_ptr< std::istream > isp = std::unique_ptr< std::istream >(new strict_fstream::ifstream(file));
        CatStream(*isp, *osp, dwBuffSize);
    }
}

int CFileUtil::Uncompress(const std::string& rstrIn, const std::string& rstrOut, uint32_t dwBuffSize, uint32_t dwCpuCore)
{
    std::string strMidFile;
    GetTmpMiddleFile(strMidFile, false);
    assert(!strMidFile.empty());
    //文件进行解压缩
    std::unique_ptr< std::ofstream > ofsp;
    std::ostream * osp = &std::cout;
    ofsp = std::unique_ptr< std::ofstream >(new strict_fstream::ofstream(strMidFile));
    osp = ofsp.get();
    std::unique_ptr< std::istream > isp = std::unique_ptr< std::istream >(new zio::ifstream(rstrIn, dwBuffSize));
    CatStream(*isp, *osp, dwBuffSize);

    //解档
    Dearchive(strMidFile, rstrOut, dwBuffSize);

    //删除临时文件
    fs::remove(fs::path(strMidFile));
    return 0;
}

int CFileUtil::EncodeFile(const std::string& rstrSource, const std::string& rstrEncodeFileDir, const std::string& rstrKey, std::string& rstrOutFile)
{
    //加密后文件名称：源文件文件名+.spc
    std::cout << "encodefile:" << rstrSource << std::endl;
    return EncodeFile2(rstrSource, rstrEncodeFileDir, true, rstrOutFile);
}

int CFileUtil::DecodeFile(const std::string& rstrEncodeFile, const std::string& rstrDecodeFileDir, const std::string& rstrKey, std::string& rstrOutFile)
{
    return EncodeFile2(rstrEncodeFile, rstrDecodeFileDir, false, rstrOutFile);
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
        delete[] szBuff;
        return true;
    }
    else
    {
        return false;
    }
    
}

int CFileUtil::EncodeFile2(const std::string& rstrSource, const std::string& rstrOut, bool bEncode, std::string& rstrOutFile)
{
    //判断输出文件夹是否存在如果不存在，则创建
    fs::path filePath(rstrOut);
    if(!fs::exists(filePath))
    {
        fs::create_directories(filePath);
    }
    
    std::ifstream in(rstrSource, std::ifstream::in | std::ifstream::binary);
    std::ofstream out;
    std::string strExt(".spc");
    int nError = 0;
    do
    {
        if(!in.is_open())
        {
            nError = 1;
            break;
        }
        
        std::string strOutFile;
        EncodeHeaderInfo ehInfo;
        //首先读取加密文件头信息
        in.read((char*)&ehInfo, sizeof(ehInfo));

        if(bEncode)
        {
            //
            
            if(std::string(ehInfo.szExt) == strExt)
            {
                //已经加密过
                nError = 3;
                break;
            }
            else
            {
                //恢复文件指针到文件头部
                strOutFile = fs::path(rstrSource).filename().string();
                strOutFile.append(strExt);
                strOutFile = (filePath / strOutFile).string();
                in.seekg(0, in.beg);
            }
        }
        else
        {
            if(std::string(ehInfo.szExt) != strExt)
            {
                nError = 4;
                break;
            }
            strOutFile = (filePath / ehInfo.szFilename).string();
        }
        std::cout << "source:" << rstrSource << "out:" << strOutFile << std::endl;
        out.open(strOutFile, std::ofstream::out | std::ofstream::trunc);
        if(!out.is_open())
        {
            nError = 2;
            break;
        }

        if(bEncode)
        {
            strncpy(&ehInfo.szFilename[0], fs::path(rstrSource).filename().string().c_str(), rstrSource.length());
            strncpy(&ehInfo.szFilename[0] + fs::path(rstrSource).filename().string().length(), "\0", 1);
            strncpy(&ehInfo.szExt[0], strExt.c_str(), strExt.length());
            strncpy(&ehInfo.szExt[0] + strExt.length(), "\0", 1);
            out.write((char*)&ehInfo, (size_t)sizeof(EncodeHeaderInfo));
        }
        
        //open file normally
        char c;
        while(in.peek() != EOF)
        {
            in.get(c);
            out << (char)(~c);
        }
        rstrOutFile = strOutFile;
    }while(false);

    if(nError == 0 )
    {
        std::cout << (bEncode ? "encode" : "decode") << " file successfully" << std::endl;
    }
    else
    {
        std::cout << (bEncode ? "encode" : "decode") << " file fail, error code: "  << nError << std::endl;
    }
    in.close();
    out.close();
    return nError;
}

int CFileUtil::Archive(const std::vector<std::string>& rVecFile, const std::string& rstrOut, uint32_t dwBuffSize)
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
                            ArchiveOneFileOrDir(dir->path().string(), oArcFile, dwBuffSize);                            
                        }
                    }
                    else if(fs::is_regular_file(path))
                    {
                        std::cout << "is file: " << path.string() << std::endl;
                        ArchiveOneFileOrDir(path.string(), oArcFile, dwBuffSize); 
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
                    DearchiveOneFileOrDir(iArcFile, rstrOut, dwBuffSize);
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


int CFileUtil::ArchiveOneFileOrDir(const std::string& rstrSource, std::ofstream& rofArchiveFile, uint32_t dwBuffSize)
{
    assert(dwBuffSize != 0);
    fs::path path(rstrSource);
    if(fs::is_regular_file(path))
    {
        std::ifstream iReadyFile(rstrSource, std::ifstream::binary);

        //该缓冲区会被反复写入，但都是根据指定字节长度读取或者写入，因此不用每次memset(pszBuff.get(), 0, dwBuffSize);
        std::unique_ptr<char> pszBuff = std::unique_ptr<char>(new char[dwBuffSize]);
        
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
                    rofArchiveFile.write(szBuff, dwBuffSize);
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
            iReadyFile.read(szBuff, dwBuffSize);
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

int CFileUtil::DearchiveOneFileOrDir(std::ifstream& rifSource, const std::string& rstrOut, uint32_t dwBuffSize)
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
            std::unique_ptr<char> pszBuff = std::unique_ptr<char>(new char[dwBuffSize]);
            char* szBuff = pszBuff.get();
            if(stFileInfo.ullFSize > dwBuffSize)
            {
                uint32_t dwNeedReadLoop = ceil(stFileInfo.ullFSize * 1.0 / dwBuffSize);
                uint32_t dwCnt = dwNeedReadLoop;
                std::cout << "loop:" << dwNeedReadLoop << std::endl;
                while(dwCnt--)
                {
                    std::cout << "current loop:" << dwCnt << std::endl;
                    if(dwCnt != 1)
                    {
                        rifSource.read(szBuff, dwBuffSize);
                        oFile.write(szBuff, dwBuffSize);
                    }
                    else
                    {
                        uint32_t dwRestBytes = stFileInfo.ullFSize - (dwNeedReadLoop - 1) * dwBuffSize;
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