#pragma once 
#include "FileUtil4Zlib.h"

class CFileUncompresser : public CFileUtil4Zlib
{
public:
	int 
	Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile);

};