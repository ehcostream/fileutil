#include "FileUncompresserAsync.h"
#include "ThreadPool.h"
int CFileUncompresserAsync::Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile)
{
	assert(rvecFiles.size() == 1);
	CThreadPool::Instance().Post(boost::bind(&CFileUncompresserAsync::Uncompress, this, rvecFiles.front(), rstrOutDir));
	std::cout << __FILE__ << "\t" << __FUNCTION__ << std::endl;
	return 0;
}
