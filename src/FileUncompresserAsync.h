#pragma once 
#include "FileUtil4Zlib.h"

class CFileUncompresserAsync : public CFileUtil4Zlib
{
private:
	int 
	Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile);
	
};