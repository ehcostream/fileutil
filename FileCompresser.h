#pragma once 
#include "FileUtil4Zlib.h"

class CFileCompresser : public CFileUtil4Zlib
{
public:
	int 
	Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile);
};