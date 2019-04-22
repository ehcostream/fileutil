#ifndef __FILE_UTIL_GENERATOR_BASE__
#define __FILE_UTIL_GENERATOR_BASE__

#include "Uncopyable.h"
#include "FileUtilGeneratorInterface.h"
#include "VersionInfo.h"

class CFileUtilGeneratorBase 
	: public CUncopyable, 
	  public CFileUtilGeneratorInterface
{
public:
	//创建压缩工具
	virtual CFileUtilBase* CreateCompresser() override;

	//创建解压工具
	virtual CFileUtilBase* CreateUncompresser(const std::string& rstrInFile) override;

	//创建加密工具
	virtual CFileUtilBase* CreateEncoder() override;

	//创建解密工具
	virtual CFileUtilBase* CreateDecoder(const std::string& rstrInFile) override;

protected:
	virtual CVersionInfo GetFileUtilVer(const std::string& rstrFile);

	CFileUtilGeneratorBase() = default;
	~CFileUtilGeneratorBase() = default;
};

#endif