#include "FileUncompresser.h"

int CFileUncompresser::Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile)
{
    assert(rvecFiles.size() == 1);
	//return Uncompress(rvecFiles.front(), rstrOutDir);
	return UncompressWithGrpc(rvecFiles.front(), rstrOutDir);
}
