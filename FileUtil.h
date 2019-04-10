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

class CFileUtil
{
public:
    static int Compress(const std::vector<std::string>& rVecFile, const std::string& rstrOut, uint32_t dwBuffSize, uint32_t dwCpuCore = 1);

    static int Uncompress(const std::string& rstrIn, const std::string& rstrOut, uint32_t dwBuffSize,  uint32_t dwCpuCore = 1);

    static int EncodeFile(const std::string& rstrSource, const std::string& rstrEncodeFileDir);

    static int DecodeFile(const std::string& rstrEncodeFile, const std::string& rstrDecodeFileDir);

    static bool GetFileName(const std::string& rstrFilePath, std::string& rstrFileName);

private:
    static bool CatStream(std::istream& ris, std::ostream& ros, uint32_t dwBuffSize);

    static int ReverseStream(const std::string& rstrSource, const std::string& rstrOut);

    //rstrOut为具体文件名，包含路径
    static int Archive(const std::vector<std::string>& rVecFile, const std::string& rstrOut, uint32_t dwBuffSize);

    //rstrOut为解档文件夹
    static int Dearchive(const std::string& rstrArchiveFile, const std::string& rstrOut, uint32_t dwBuffSize);

    //归档一个文件或文件夹
    static int ArchiveOneFileOrDir(const std::string& rstrSource, std::ofstream& rofArchiveFile, uint32_t dwBuffSize);

    //解档一个块文件
    static int DearchiveOneFileOrDir(std::ifstream& rifSource, const std::string& rstrOut, uint32_t dwBuffSize);

};

#endif
//FILEUTIL_H