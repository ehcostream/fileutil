#include "FileUtil4Zlib.h"

int CFileUtil4Zlib::Compress(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, std::string& rstrOutFile)
{
    std::cout << "Compress" << std::endl;
    assert(rvecFiles.size() > 0);
    
    if(fs::is_regular_file(rvecFiles.front()) && rvecFiles.size() == 1)
    {
        int nParseResult = 0;
        std::unique_ptr< std::ifstream > ifsp = std::unique_ptr< std::ifstream >(new strict_fstream::ifstream(rvecFiles.front()));
        std::istream * isp = ifsp.get();
        FileHead stHead;
        CFileUtilHead::Parse(*isp, nParseResult, stHead);
        if(std::string(stHead.szExt) == std::string(".zb"))
        {
            ifsp->close();
            //重复压缩
            return 1;
        }
        ifsp->close();
    }
    std::cout << CCustomParamManager::Instance().GetCpuCore() << std::endl;
    std::cout << CCustomParamManager::Instance().GetBuffSize() << std::endl;
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
    for(const auto& file : rvecFiles)
    {
        std::cout << file << std::endl;
    }
    if(rvecFiles.size() > 1)
    {
        //如果有多个文件/夹，以第一个文件的父目录名称为文件名建立压缩文件
        fs::path ptmp(fs::absolute(file));
        if(std::string(&file.back()) == std::string("/") || std::string(&file.back()) == std::string("\\"))
        {
            file.pop_back();
        }
        strTargetFile = fs::canonical(fs::absolute(file).parent_path()).filename().string();
    }
    else
    {
        fs::path path(file);
        std::cout << "--->" << file << std::endl;
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

    std::cout << CCustomParamManager::Instance().GetCpuCore() << "," << CCustomParamManager::Instance().GetBuffSize() << std::endl;
    if(CCustomParamManager::Instance().GetCpuCore() > 1)
    {
        //多线程压缩
        CompressWithMT(strMidFile, outFilePath.string());
    }
    else
    {
        //调用zlib进行压缩
        std::string strTmpFile;
        do
        {
            GetTmpMiddleFile(strTmpFile, true);
            std::unique_ptr< std::ostream> osp = std::unique_ptr< std::ostream >(new zio::ofstream(strTmpFile, CCustomParamManager::Instance().GetBuffSize()));
            std::unique_ptr< std::ifstream > ifsp = std::unique_ptr< std::ifstream >(new strict_fstream::ifstream(strMidFile));
            zio::ofstream* ofsp = static_cast<zio::ofstream*>(osp.get());
            //附加文件头信息
            
            CatStream(*ifsp, *osp);
            ifsp->close();
            
        }while(false);
        std::ifstream in(strTmpFile, std::ifstream::binary);
        std::ofstream out(outFilePath.string(), std::ofstream::binary);
        CFileUtilHead::Attach(out, outFilePath.string());
        CatStream(in, out);

        in.close();
        out.close();
        //删除临时文件
        std::cout << "remove strMidFile " << strMidFile << std::endl;
        fs::remove(fs::path(strMidFile));
        std::cout << "remove strTmpFile " << strTmpFile << std::endl;
        fs::remove(fs::path(strTmpFile));
        
    }
    
	return 0;
}

int CFileUtil4Zlib::Uncompress(const std::string& rstrIn, const std::string& rstrOutDir)
{
	std::string strMidFile;
    GetTmpMiddleFile(strMidFile, false);
    assert(!strMidFile.empty());
    std::cout << this << std::endl;
    //文件进行解压缩
    std::unique_ptr< std::ofstream > ofsp = std::unique_ptr< std::ofstream >(new strict_fstream::ofstream(strMidFile));
    std::ostream * osp = ofsp.get();
    std::cout << __FUNCTION__ << "," << CCustomParamManager::Instance().GetBuffSize() << "," << rstrIn << std::endl;
    
    std::unique_ptr< std::istream > isp = std::unique_ptr< std::istream >(new zio::ifstream(rstrIn, CCustomParamManager::Instance().GetBuffSize()));
    zio::ifstream* ifsp = static_cast<zio::ifstream*>(isp.get());
    FileHead stHead;
    memset(&stHead, '\0', sizeof(stHead));
    int nError = 0;
    std::ifstream in(rstrIn, std::ifstream::in | std::ifstream::binary);
    do
    {
        //检测文件是否有效
        CFileUtilHead::Parse(in, nError, stHead);
        if(nError != 0)
        {
            nError = 1;
            break;
        }
        if(std::string(stHead.szExt) != std::string(".zb"))
        {
            nError = 2;
            break;
        }

        ifsp->GetStreamBuf()->pubseekoff(sizeof(stHead), ifsp->beg);
        CatStream(*isp, *osp);
        ofsp->close();

        //解档
        nError = Dearchive(strMidFile, rstrOutDir);
        std::cout <<  nError << std::endl;
    }while(false);
    in.close();
    //删除临时文件
    fs::remove(fs::path(strMidFile));
    std::cout << __FUNCTION__ << ", end" << std::endl;
	return nError;
}

int CFileUtil4Zlib::CompressWithMT(const std::string rstrAchiveFile, const std::string& rstrOutDir)
{
    //切割为多个多文件
    //开启线程池，加入多个线程，根据buffsize平均分配，同时压缩文件
    //等待所有线程完成，将结果写入目标文件中
    assert(CCustomParamManager::Instance().GetBuffSize() > 0 && CCustomParamManager::Instance().GetCpuCore() > 1);

    uint32_t dwThreadCnt = CCustomParamManager::Instance().GetCpuCore();
    std::vector<std::string> vecFiles;
    std::vector<std::string> vecResultFiles;

    int nError = 0;
    do
    {
        
        CutFileIntoPieces(rstrAchiveFile, vecFiles, dwThreadCnt);
        if(vecFiles.size() != dwThreadCnt)
        {
            std::cout << "vecfiles size invalid:" << vecFiles.size() << std::endl;;
            nError = 1;
            break;
        }

        std::vector<ThreadParam> vecThreadList;
        uint32_t dwGeneralAvg = floor(CCustomParamManager::Instance().GetBuffSize() / dwThreadCnt);
        boost::thread_group threadPool;
        //初始化线程池
        for(uint32_t i = 0; i< dwThreadCnt; i++)
        {
            ThreadParam stThreadParam;
            stThreadParam.threadSeq = i;
            if(i == dwThreadCnt -1)
            {
                stThreadParam.ullBuffSize = CCustomParamManager::Instance().GetBuffSize() - dwGeneralAvg * (dwThreadCnt - 1);
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
            auto threadCallBack = &CFileUtil4Zlib::CompressAFile;
            threadPool.create_thread(boost::bind(threadCallBack, this, std::ref(param)));
        }

        threadPool.join_all();    
        
        std::cout << "thread finished" << std::endl;
        //处理完成重组数据
        
        for(const auto& param : vecThreadList)
        {
            vecResultFiles.emplace_back(param.strOutFile);
        }

        std::cout << "ready to combain files" << std::endl;
        //重组数据
        CombainFiles(vecResultFiles, rstrOutDir);

        
    }while(false);
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

void CFileUtil4Zlib::CompressAFile(ThreadParam& stParam)
{
    GetTmpMiddleFile(stParam.strOutFile, true, 2, stParam.threadSeq);
    std::cout << "  thread source--->" << stParam.strSource << "  thread result--->" << stParam.strOutFile << " buff size-->" << stParam.ullBuffSize << std::endl;
    std::unique_ptr< std::ostream > osp = std::unique_ptr< std::ostream >(new zio::ofstream(stParam.strOutFile, stParam.ullBuffSize));
    std::unique_ptr< std::ifstream > ifsp = std::unique_ptr< std::ifstream >(new strict_fstream::ifstream(stParam.strSource));
    CatStream(*ifsp, *osp);
    ifsp->close();
    std::cout << "  thread result--->" << stParam.strOutFile << std::endl;
}
