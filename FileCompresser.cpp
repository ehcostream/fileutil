#include "FileCompresser.h"

int CFileCompresser::Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile)
{
	
    std::cout << "Compress" << std::endl;
    assert(rvecFiles.size() > 0);
    //归档
    std::string strMidFile;
    GetTmpMiddleFile(strMidFile, true);
    assert(!strMidFile.empty());
    Archive(rvecFiles, strMidFile);

    fs::path outDir(rstrOutDir);
    if(!fs::exists(outDir))
    {
        fs::create_directories(outDir);
    }

    fs::path outFilePath(outDir);
    std::string strExtension = ".ar.zb";
    std::string strTargetFile;

    auto file = rvecFiles.front();
    if(rvecFiles.size() > 1)
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
    if(m_dwCpuCore > 1)
    {
        //多线程压缩
        CompressWithMT(strMidFile, m_ullBuffSize, m_dwCpuCore, outFilePath.string());
    }
    else
    {
        //调用zlib进行压缩
        std::unique_ptr< std::ostream > osp = std::unique_ptr< std::ostream >(new zio::ofstream(outFilePath.string(), m_ullBuffSize));

        std::unique_ptr< std::ifstream > ifsp;
        std::istream * isp = &std::cin;

        ifsp = std::unique_ptr< std::ifstream >(new strict_fstream::ifstream(strMidFile));
        isp = ifsp.get();
        CatStream(*isp, *osp);

        //删除临时文件
        fs::remove(fs::path(strMidFile));
    }

    
    return 0;
}


int CFileCompresser::CompressWithMT(const std::string rstrAchiveFile, uint32_t m_ullBuffSize, uint32_t m_dwCpuCore, const std::string& rstrOut)
{
    //切割为多个多文件
    //开启线程池，加入多个线程，根据buffsize平均分配，同时压缩文件
    //等待所有线程完成，将结果写入目标文件中
    assert(m_ullBuffSize > 0 && m_dwCpuCore > 1);

    uint32_t dwThreadCnt = m_dwCpuCore;
    std::vector<std::string> vecFiles;
    CutFileIntoPieces(rstrAchiveFile, vecFiles, dwThreadCnt);
    if(vecFiles.size() != dwThreadCnt)
    {
        std::cout << "vecfiles size:" << vecFiles.size() << std::endl;;
        return 1;
    }

    std::vector<ThreadParam> vecThreadList;
    uint32_t dwGeneralAvg = floor(m_ullBuffSize / dwThreadCnt);
    boost::thread_group threadPool;
    //初始化线程池
    for(uint32_t i = 0; i< dwThreadCnt; i++)
    {
        ThreadParam stThreadParam;
        if(i == dwThreadCnt -1)
        {
            stThreadParam.ullBuffSize = m_ullBuffSize - dwGeneralAvg * (dwThreadCnt - 1);
        }
        else
        {
            stThreadParam.ullBuffSize = dwGeneralAvg;
        }
        stThreadParam.strSource = vecFiles[i];
        vecThreadList.emplace_back(stThreadParam);
    }

    std::cout << "thread prepare..." << vecThreadList.size() << std::endl;
    //多线程同时压缩数据
    for(auto& param : vecThreadList)
    {
    	auto threadCallBack = &CFileCompresser::CompressAFile;
        threadPool.create_thread(boost::bind(threadCallBack, this, &param));
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
    CombainFiles(vecResultFiles, rstrOut);

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

void CFileCompresser::CompressAFile(void* pParam)
{
	assert(pParam != nullptr);
    ThreadParam* pTParam = (ThreadParam*)pParam;
    GetTmpMiddleFile(pTParam->strOutFile, true, 2);
    std::cout << "  thread source--->" << pTParam->strSource << std::endl;
    std::cout << "  thread result--->" << pTParam->strOutFile << std::endl;
    std::unique_ptr< std::ostream > osp = std::unique_ptr< std::ostream >(new zio::ofstream(pTParam->strOutFile, pTParam->ullBuffSize));
    std::unique_ptr< std::ifstream > ifsp = std::unique_ptr< std::ifstream >(new strict_fstream::ifstream(pTParam->strSource));
    std::istream * isp = ifsp.get();
    CatStream(*isp, *osp);
}