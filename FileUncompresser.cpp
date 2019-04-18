#include "FileUncompresser.h"

int CFileUncompresser::Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile)
{
	assert(rvecFiles.size() == 1);
	std::string strMidFile;
    GetTmpMiddleFile(strMidFile, false);
    assert(!strMidFile.empty());
    //文件进行解压缩
    std::unique_ptr< std::ofstream > ofsp;
    std::ostream * osp = &std::cout;
    ofsp = std::unique_ptr< std::ofstream >(new strict_fstream::ofstream(strMidFile));
    osp = ofsp.get();
    std::unique_ptr< std::istream > isp = std::unique_ptr< std::istream >(new zio::ifstream(rvecFiles.front(), m_ullBuffSize));
    CatStream(*isp, *osp);

    //解档
    Dearchive(strMidFile, rstrOutDir);

    //删除临时文件
    fs::remove(fs::path(strMidFile));
	return 0;
}
