#pragma once
#include "Zlibutil.h"
class CSymCrypto
{
protected:
	int SymEncode(const std::string& rstrSource, const std::string& rstrOut, const std::string& rstrKey, bool bEncode, std::string& rstrOutFile);
};