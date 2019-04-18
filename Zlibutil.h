#ifndef __ZLIB_UTIL_H__
#define __ZLIB_UTIL_H__

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cstdio>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <vector>
#include <cmath>
#include <assert.h>
#include <chrono>

#include <boost/thread/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <unordered_map>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/any.hpp>
#include <zlib.h>
#include "zio.cpp"
#include "Version.h"
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

//线程参数
struct ThreadParam
{
    uint32_t dwBuffSize;
    std::string strSource;
    std::string strOutFile;
};
#endif