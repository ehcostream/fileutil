#include "FileEncoderAsync.h"

int CFileEncoderAsync::Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile)
{
	assert(rvecFiles.size() == 1);
	CThreadPool::Instance().Post(boost::bind(&CFileEncoderAsync::SymEncode, this, rvecFiles.front(), rstrOutDir, true, std::ref(rstrOutFile)));
	std::cout << __FILE__ << "\t" << __FUNCTION__ << std::endl;
	return 0;
}
