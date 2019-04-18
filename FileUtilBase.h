#pragma once 
#include "Zlibutil.h"

class CFileUtilBase
{
public:
	CFileUtilBase() = default;
	virtual ~CFileUtilBase() = default;

public:
	virtual int 
	Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile) = 0;

    virtual void SetSysParam(uint32_t dwCpuCore, uint64_t ullBufferSize);

protected:
//常用的文件操作
	//文件内容复制
	bool CatStream(std::istream& ris, std::ostream& ros);

    //归档
    int Archive(const std::vector<std::string>& rVecFile, const std::string& rstrOut);

    //解档
    int Dearchive(const std::string& rstrArchiveFile, const std::string& rstrOut);

    //归档一个块文件
    int ArchiveOneFileOrDir(const std::string& rstrSource, std::ofstream& rofArchiveFile);

    //解档一个块文件
    int DearchiveOneFileOrDir(std::ifstream& rifSource, const std::string& rstrOut);

    //获取一个临时(解)归档文件名
    void GetTmpMiddleFile(std::string& rstrAchiveFile, bool bAchive, int nType = 0);

    //一个文件均分为多个子文件
    int CutFileIntoPieces(const std::string& rstrIn, std::vector<std::string>& rVecOutFiles, uint32_t dwBlcok);

    //合并多个文件为一个文件
    void CombainFiles(const std::vector<std::string>& rVecInFiles, const std::string& rstrOutFile);


protected:
    uint32_t m_dwCpuCore = CPU_CORE_DEFAULE;
    uint64_t m_ullBuffSize = BUFF_SZE;

private:
    static const uint32_t CPU_CORE_DEFAULE = 1;
    static const uint64_t BUFF_SZE = 1 <<  20;
};