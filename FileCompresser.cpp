#include "FileCompresser.h"

int CFileCompresser::Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile)
{

	return Compress(rvecFiles, rstrOutDir, rstrOutFile);
}

