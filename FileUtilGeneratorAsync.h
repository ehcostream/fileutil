#pragma once
#include "Singleton.h"
#include "FileUtilGeneratorBase.h"

class CFileUtilBase;

class CFileUtilGeneratorAsync : public CFileUtilGeneratorBase , public CSingleton<CFileUtilGeneratorAsync>
{
public:
	friend CSingleton;
	//创建压缩工具
	virtual CFileUtilBase* CreateCompresser();

	//创建解压工具
	virtual CFileUtilBase* CreateUncompresser(const std::string& rstrInFile);

	//创建加密工具
	virtual CFileUtilBase* CreateEncoder();

	//创建解密工具
	virtual CFileUtilBase* CreateDecoder(const std::string& rstrInFile);

private:
	int GetCpuCoresAvaiable();

private:
	CFileUtilGeneratorAsync();
	~CFileUtilGeneratorAsync() = default;
};