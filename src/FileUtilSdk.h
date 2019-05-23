#ifndef __FILE_UTIL_SDK_H__
#define __FILE_UTIL_SDK_H__

#define FILEUTIL_INTERNAL
#include "fconf.h"
#include "FileUtilGeneratorBase.h"
#include "FileUtilBase.h"

namespace futil
{

//创建工具类工厂
	FEXTERN CFileUtilGeneratorBase* CreateFactory(int async, const char* szConfig);

//主线程等待异步操作结束
	FEXTERN void WaitAsync();
}

#endif