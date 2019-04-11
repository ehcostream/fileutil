/*
 * FileUtil.h
 *
 *  Created on: 2019年4月3日
 *      Author: Edric
 *      Brief:  文件相关操作工具
 */

#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <string>
#include <memory>
#include <cstdio>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <zlib.h>
#include <cmath>
#include <assert.h>
#include <chrono>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "zio.cpp"

namespace fs = boost::filesystem;

//进行归档的文件的头部信息
enum FileType
{
    FT_UNKNOW = 0x00,
    FT_FILE = 0x01,
    FT_DIR = 0x02,
    FT_MAX
};

struct FileInfo
{
    //文件类型
    char bFType;
    //文件大小，满足文件大小大于4G的情况
    uint64_t ullFSize;
    //文件路径
    char szFPath[255];
};

//加密文件头信息
struct EncodeHeaderInfo
{
    //加密文件类型
    char szExt[10];
    //文件原名称
    char szFilename[255];
};

class CFileUtil
{
public:
    static int Compress(const std::vector<std::string>& rVecFile, const std::string& rstrOut, uint32_t dwBuffSize, uint32_t dwCpuCore = 1);

    static int Uncompress(const std::string& rstrIn, const std::string& rstrOut, uint32_t dwBuffSize,  uint32_t dwCpuCore = 1);

    static int EncodeFile(const std::string& rstrSource, const std::string& rstrEncodeFileDir);

    static int DecodeFile(const std::string& rstrEncodeFile, const std::string& rstrDecodeFileDir);
private:
    //将文件流输出到加密/解密流中
    static bool CatStream(std::istream& ris, std::ostream& ros, uint32_t dwBuffSize);

    //加(解)密文件
    static int EncodeFile2(const std::string& rstrSource, const std::string& rstrOut, bool bEncode);

    //rstrOut为文件夹
    static int Archive(const std::vector<std::string>& rVecFile, const std::string& rstrOut, uint32_t dwBuffSize);

    //rstrOut为文件夹
    static int Dearchive(const std::string& rstrArchiveFile, const std::string& rstrOut, uint32_t dwBuffSize);

    //归档一个块文件
    static int ArchiveOneFileOrDir(const std::string& rstrSource, std::ofstream& rofArchiveFile, uint32_t dwBuffSize);

    //解档一个块文件
    static int DearchiveOneFileOrDir(std::ifstream& rifSource, const std::string& rstrOut, uint32_t dwBuffSize);

    //获取一个临时(解)归档文件名
    static void GetTmpMiddleFile(std::string& rstrAchiveFile, bool bAchive);

};

#endif
//FILEUTIL_H