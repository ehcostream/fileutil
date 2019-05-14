#ifndef __FILE_UTIL_SDK_H__
#define __FILE_UTIL_SDK_H__
#include "fconf.h"
#include "FileUtilGeneratorBase.h"
#include "FileUtilBase.h"
namespace futil
{

//创建工具类工厂
	FEXTERN CFileUtilGeneratorBase* CreateFactory(int async);

//主线程等待异步操作结束
	FEXTERN void WaitAsync();
}

#endif