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
	//TODO 兼容旧版本创建的文件
	return new CFileUncompresser();
}

//创建加密工具
CFileUtilBase* CFileUtilGenerator::CreateEncoder()
{
	return new CFileEncoder();
}

//创建解密工具
CFileUtilBase* CFileUtilGenerator::CreateDecoder(const std::string& rstrInFile)
{
	//兼容模式TODO
	// CVersionInfo& rstVersionInfo = GetFileUtilVer(rstrInFile);
	// if(rstVersionInfo.GetMiniorVer() == Version::MINOR_VERSION)
	// {
	// 	//使用当前最新的
	// 	return new CFileDecoder();
	// }
	// else if(rstVersionInfo.GetMiniorVer() == 50)
	// {
	// 	return new CFileDecoderOld();
	// }
	// else
	// {
	// 	return nullptr;
	// }
	return new CFileDecoder();
}