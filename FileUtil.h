/*
 * FileUtil.h
 *
 *  Created on: 2019年4月3日
 *      Author: Edric
 *      Brief:  文件相关操作工具
 */

#ifndef __FILEUTIL_H__
#define __FILEUTIL_H__
#include "Zlibutil.h"

class CFileUtil
{
public:
    static int Compress(const std::vector<std::string>& rVecFile, const std::string& rstrOut, uint32_t dwBuffSize, uint32_t dwCpuCore, std::string& rstrOutFile);
    
    static int Uncompress(const std::string& rstrIn, const std::string& rstrOut, uint32_t dwBuffSize,  uint32_t dwCpuCore = 1);

    static int EncodeFile(const std::string& rstrSource, const std::string& rstrEncodeFileDir, const std::string& rstrKey, std::string& rstrOutFile);

    static int DecodeFile(const std::string& rstrEncodeFile, const std::string& rstrDecodeFileDir, const std::string& rstrKey, std::string& rstrOutFile);

private:
    //多线程压缩
    static int CompressWithMT(const std::string rstrAchiveFile, uint32_t dwBuffSize, uint32_t dwCpuCore, const std::string& rstrOut);

    //单个线程压缩流
    static void CompressAFile(void* pParam);

    //加(解)密文件
    static int EncodeFile2(const std::string& rstrSource, const std::string& rstrOut, bool bEncode, std::string& rstrOutFile);

    //将文件流输出到加密/解密流中
    static bool CatStream(std::istream& ris, std::ostream& ros, uint32_t dwBuffSize);

    //rstrOut为文件夹
    static int Archive(const std::vector<std::string>& rVecFile, const std::string& rstrOut, uint32_t dwBuffSize);

    //rstrOut为文件夹
    static int Dearchive(const std::string& rstrArchiveFile, const std::string& rstrOut, uint32_t dwBuffSize);

    //归档一个块文件
    static int ArchiveOneFileOrDir(const std::string& rstrSource, std::ofstream& rofArchiveFile, uint32_t dwBuffSize);

    //解档一个块文件
    static int DearchiveOneFileOrDir(std::ifstream& rifSource, const std::string& rstrOut, uint32_t dwBuffSize);

    //获取一个临时(解)归档文件名
    static void GetTmpMiddleFile(std::string& rstrAchiveFile, bool bAchive, int nMT = 0);

    //一个文件均分为多个子文件
    static int CutFileIntoPieces(const std::string& rstrIn, std::vector<std::string>& rVecOutFiles, uint32_t dwBlcok);

    //合并多个文件为一个文件
    static void CombainFiles(const std::vector<std::string>& rVecInFiles, const std::string& rstrOutFile, uint32_t dwBuffSize);
};

#endif
//__FILEUTIL_H__