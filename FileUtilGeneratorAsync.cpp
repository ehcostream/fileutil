#include "FileUtilGeneratorAsync.h"
#define ASYNC
#include "FileUtilities.h"

CFileUtilBase* CFileUtilGeneratorAsync::CreateCompresser()
{
	return new CFileCompresserAsync();
}

//创建解压工具
CFileUtilBase* CFileUtilGeneratorAsync::CreateUncompresser(const std::string& rstrInFile)
{
	//TODO 兼容旧版本创建的文件
	return new CFileUncompresserAsync();
}

//创建加密工具
CFileUtilBase* CFileUtilGeneratorAsync::CreateEncoder()
{
	return new CFileEncoderAsync();
}

//创建解密工具
CFileUtilBase* CFileUtilGeneratorAsync::CreateDecoder(const std::string& rstrInFile)
{
	return new CFileDecoderAsync();
}