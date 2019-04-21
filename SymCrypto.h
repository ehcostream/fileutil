#pragma once
#include "Zlibutil.h"
class CSymCrypto
{
protected:
	int SymEncodeOld(const std::string& rstrSource, const std::string& rstrOut, const std::string& rstrKey, bool bEncode, std::string& rstrOutFile);
	int SymEncode(const std::string& rstrSource, const std::string& rstrOut, const std::string& rstrKey, bool bEncode, std::string& rstrOutFile);
};