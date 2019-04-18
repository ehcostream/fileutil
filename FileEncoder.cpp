#include "FileEncoder.h"

int CFileEncoder::Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile)
{
	assert(rvecFiles.size() == 1);
	SymEncode(rvecFiles.front(), rstrOutDir, true, rstrOutFile);
	return 0;
}
