#include "FileEncoder.h"

int CFileEncoder::Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile)
{
	assert(rvecFiles.size() == 1);
	const char* szKey = (const char*)pExParam;
	assert(szKey != nullptr);
	std::string strKey(szKey);
	return SymEncode(rvecFiles.front(), rstrOutDir, strKey, true, rstrOutFile);
}
