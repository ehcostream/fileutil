#include "FileUtilGeneratorBase.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "FileUtilHead.h"
#include "CustomParamManager.h"

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

void CFileUtilGeneratorBase::Set(uint32_t dwCpuCore, uint64_t ullBuffSize)
{
	CCustomParamManager::Instance().Set(dwCpuCore, ullBuffSize);
}

CVersionInfo CFileUtilGeneratorBase::GetFileUtilVer(const std::string& rstrFile)
{
	FileHead stHead;
	int nError = CFileUtilHead::GetData(rstrFile, stHead);
	if(nError != 0)
	{
		return CVersionInfo(0,0,0);
	}
	else
	{
		std::string strVersion(stHead.szVersion);
		std::vector<std::string> stResult;
		boost::split(stResult, strVersion, boost::is_any_of("-."));
		if(stResult.size() == 3)
		{
			return CVersionInfo(
				boost::lexical_cast<uint32_t>(stResult[0]), 
				boost::lexical_cast<uint32_t>(stResult[1]), 
				boost::lexical_cast<uint32_t>(stResult[2]));
		}
		else
		{
			return CVersionInfo(0,0,0);
		}
		
	}
}