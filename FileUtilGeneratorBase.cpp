#include "FileUtilGeneratorBase.h"
#include "Version.h"
#include "VersionInfo.h"

CFileUtilBase* CFileUtilGeneratorBase::CreateCompresser()
{ 
	return nullptr; 
}

//创建解压工具
CFileUtilBase* CFileUtilGeneratorBase::CreateUncompresser(const std::string& rstrInFile)
{ 
	return nullptr; 
}

//创建加密工具
CFileUtilBase* CFileUtilGeneratorBase::CreateEncoder()
{ 
	return nullptr; 
}

//创建解密工具
CFileUtilBase* CFileUtilGeneratorBase::CreateDecoder(const std::string& rstrInFile)
{ 
	return nullptr; 
}

CVersionInfo CFileUtilGeneratorBase::GetFileUtilVer(const std::string& rstrFile)
{
	//TODO读取文件头信息
	return CVersionInfo(Version::MAJOR_VERSION, Version::MINOR_VERSION, Version::PATCH_VERSION);
}