#pragma once 
#include "FileUtilBase.h"
class CFileDecoder : public CFileUtilBase
{
public:
	int 
	Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile);
};