#include "FileDecoder.h"

int CFileDecoder::Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile)
{
	assert(rvecFiles.size() == 1);
	return SymEncode(rvecFiles.front(), rstrOutDir, false, rstrOutFile);
}
