#pragma once 
#include "FileUtil4Zlib.h"

class CFileCompresserAsync : public CFileUtil4Zlib
{
private:
	int 
	Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile);
};