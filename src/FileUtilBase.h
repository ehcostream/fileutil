#ifndef __FILE_UTIL_BASE__
#define __FILE_UTIL_BASE__

#include "FileUtil.h"
class CFileUtilBase
{
public:
	CFileUtilBase()
    {
        std::cout << __FUNCTION__ << " constructor" << std::endl; 
    }
	virtual ~CFileUtilBase() = default;

public:
	virtual int 
	Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile) = 0;

protected:
//常用的文件操作
	//文件内容复制,仅限于文件压缩
	bool CatStream(std::istream& ris, std::ostream& ros);
    //归档
    int Archive(const std::vector<std::string>& rVecFile, const std::string& rstrOut);

    //解档
    int Dearchive(const std::string& rstrArchiveFile, const std::string& rstrOut);

    //归档一个块文件
    int ArchiveOneFileOrDir(const std::string& rstrSource, const std::string& rstrRoot, std::ofstream& rofArchiveFile);

    //解档一个块文件
    int DearchiveOneFileOrDir(std::ifstream& rifSource, const std::string& rstrOut);

    //获取一个临时(解)归档文件名
    void GetTmpMiddleFile(std::string& rstrAchiveFile, bool bAchive, int nType = 0, int nThreadID = -1);

    //一个文件均分为多个子文件
    int CutFileIntoPieces(const std::string& rstrIn, std::vector<std::string>& rVecOutFiles, uint32_t dwBlcok);

    //合并多个文件为一个文件
    void CombainFiles(const std::vector<std::string>& rVecInFiles, const std::string& rstrOutFile);

private:
    //构建一个以rstrRoot为根的目录路径
    void RebuildPath(const std::string& rstrSource, const std::string& rstrRoot, std::string& rstrTarget);

};

#endif