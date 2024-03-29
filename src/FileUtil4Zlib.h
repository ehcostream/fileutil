#pragma once
#include <vector>
#include <string>
#include "FileUtilBase.h"

class ThreadParam;
class CFileUtil4Zlib : public CFileUtilBase
{
public:
	virtual int 
	Execute(const std::vector<std::string>& rvecFiles, const std::string& rstrOutDir, void* pExParam, std::string& rstrOutFile) { return 0; }

protected:
    int Compress(const std::vector<std::string>& rVecFiles, const std::string& rstrOutDir, std::string& rstrOutFile);
    
    int Uncompress(const std::string& rstrIn, const std::string& rstrOutDir);

    int CompressWithGrpc(const std::vector<std::string>& rVecFile, const std::string& rstrOutDir, std::string& rstrOutFile);

    int UncompressWithGrpc(const std::string& rstrIn, const std::string& rstrOutDirs);
    
private:
	int CompressWithMT(const std::string rstrAchiveFile, const std::string& rstrOutDir);
	
    void CompressAFile(ThreadParam& stParam);

    void PrepareCompress(const std::vector<std::string>& rVecFiles, const std::string& rstrOutDir, std::string& rstrOutFile);

    int CompressFileWithZlib(const std::string& rstrSource, const std::string& rstrCompressedFile, uint64_t ullBuffSize, bool bMT = false);

    int UncompressFileWithZlib(const std::string& rstrCompressedFile, const std::string& rstrSource);

};