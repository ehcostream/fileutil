#include "FileCompresserAsync.h"

int CFileCompresserAsync::Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile)
{
	CThreadPool::Instance().Post(boost::bind(&CFileCompresserAsync::Compress, this, rvecFiles, rstrOutDir, std::ref(rstrOutFile)));
	std::cout << __FILE__ << "\t" << __FUNCTION__ << std::endl;
	return 0;
}