#pragma once 
#include "FileUtil4Zlib.h"

class CFileCompresser : public CFileUtil4Zlib
{
public:
	int 
	Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile);

private:
	int CompressWithMT(const std::string rstrAchiveFile, uint32_t dwBuffSize, uint32_t dwCpuCore, const std::string& rstrOut);
	
    void CompressAFile(void* pParam);
};