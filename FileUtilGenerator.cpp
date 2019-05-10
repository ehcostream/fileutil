#include "FileUtilGenerator.h"
#include "Singleton.h"
#include "FileUtilGeneratorBase.h"

#define SYNC
#include "FileUtilities.h"

CFileUtilBase* CFileUtilGenerator::CreateCompresser()
{
	return new CFileCompresser();
}

//创建解压工具
CFileUtilBase* CFileUtilGenerator::CreateUncompresser(const std::string& rstrInFile)
{
	//兼容模式
	CVersionInfo stVersionInfo(GetFileUtilVer(rstrInFile));
	std::cout << "file version:" << stVersionInfo.Print() << std::endl;
	if(stVersionInfo.GetMinorVer() == Version::MINOR_VERSION)
	{
		//使用当前最新的
		return new CFileUncompresser();
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
CFileUtilBase* CFileUtilGenerator::CreateEncoder()
{
	return new CFileEncoder();
}

//创建解密工具
CFileUtilBase* CFileUtilGenerator::CreateDecoder(const std::string& rstrInFile)
{
	//兼容模式
	CVersionInfo stVersionInfo(GetFileUtilVer(rstrInFile));
	std::cout << "file version:" << stVersionInfo.Print() << std::endl;
	if(stVersionInfo.GetMinorVer() == Version::MINOR_VERSION)
	{
		//使用当前最新的
		return new CFileDecoder();
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