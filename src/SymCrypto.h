#pragma once
#include "FileUtil.h"
class CSymCrypto
{
protected:
	int SymEncode(const std::string& rstrSource, const std::string& rstrOut, const std::string& rstrKey, bool bEncode, std::string& rstrOutFile);
};