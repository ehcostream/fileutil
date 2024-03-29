#include "FileDecoderAsync.h"
#include "ThreadPool.h"
int CFileDecoderAsync::Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile)
{
	assert(rvecFiles.size() == 1);
	const char* szKey = (const char*)pExParam;
	assert(szKey != nullptr);
	std::string strKey(szKey);
	CThreadPool::Instance().Post(boost::bind(&CFileDecoderAsync::SymEncode, this, rvecFiles.front(), rstrOutDir, strKey, false, std::ref(rstrOutFile)));
	std::cout << __FILE__ << "\t" << __FUNCTION__ << std::endl;
	return 0;
}
