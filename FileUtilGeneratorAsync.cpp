#include "FileUtilGeneratorAsync.h"
#define ASYNC
#include "FileUtilities.h"
#include "ThreadPool.h"
static const uint32_t THREAD_CNT = 10;

CFileUtilGeneratorAsync::CFileUtilGeneratorAsync()
{
	//初始化线程池
	CThreadPool::Instance().Init(THREAD_CNT);
}

CFileUtilBase* CFileUtilGeneratorAsync::CreateCompresser()
{
	return new CFileCompresserAsync();
}

//创建解压工具
CFileUtilBase* CFileUtilGeneratorAsync::CreateUncompresser(const std::string& rstrInFile)
{
	//兼容旧版本创建的文件
	CVersionInfo stVersionInfo(GetFileUtilVer(rstrInFile));
	std::cout << stVersionInfo.Print() << std::endl;
	if(stVersionInfo.GetMinorVer() == Version::MINOR_VERSION)
	{
		//使用当前最新的
		return new CFileUncompresserAsync();
	}
	else if(stVersionInfo.GetMinorVer() == 50)
	{
		return nullptr;/*new version*/
	}
	else
	{
		return nullptr;
	}
}

//创建加密工具
CFileUtilBase* CFileUtilGeneratorAsync::CreateEncoder()
{
	return new CFileEncoderAsync();
}

//创建解密工具
CFileUtilBase* CFileUtilGeneratorAsync::CreateDecoder(const std::string& rstrInFile)
{
	CVersionInfo stVersionInfo(GetFileUtilVer(rstrInFile));
	std::cout << stVersionInfo.Print() << std::endl;
	if(stVersionInfo.GetMinorVer() == Version::MINOR_VERSION)
	{
		//使用当前最新的
		return new CFileDecoderAsync();
	}
	else if(stVersionInfo.GetMinorVer() == 50)
	{
		return nullptr;/*new version*/
	}
	else
	{
		return nullptr;
	}
}