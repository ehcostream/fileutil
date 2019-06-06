#pragma once
#include "FileUtilBase.h"

class CSymCrypto : public CFileUtilBase
{
protected:
	int SymEncode(const std::string& rstrSource, const std::string& rstrOut, const std::string& rstrKey, bool bEncode, std::string& rstrOutFile);

	int SymEncodeWithGrpc(const std::string& rstrSource, const std::string& rstrOut, const std::string& rstrKey, bool bEncode, std::string& rstrOutFile);
};