#pragma once 
#include "SymCrypto.h"

class CFileEncoderAsync : public CSymCrypto
{
private:
	int 
	Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile);
};