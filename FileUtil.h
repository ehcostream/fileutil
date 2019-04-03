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
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <vector>
#include <zlib.h>
#include <boost/algorithm/string.hpp>
#include "zstr.cpp"

class CFileUtil
{
public:
    static const std::streamsize BUFF_SIZE = 1 << 16;

public:
    static int Compress(const std::string& rstrIn, const std::string& rstrOut);

    static int Uncompress(const std::string& rstrIn, const std::string& rstrOut);

    static int EncodeFile(const std::string& rstrSource, const std::string& rstrEncodeFileDir);

    static int DecodeFile(const std::string& rstrEncodeFile, const std::string& rstrDecodeFileDir);

    static bool GetFileName(const std::string& rstrFilePath, std::string& rstrFileName);

private:
    static void CatStream(std::istream& ris, std::ostream& ros);

    static int ReverseStream(const std::string& rstrSource, const std::string& rstrOut);
};

#endif
//FILEUTIL_H