#pragma once 
#include "FileUtilBase.h"
#include "SymCrypto.h"

class CFileDecoderAsync : public CFileUtilBase, public CSymCrypto
{
public:
	int 
	Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile);
};