#include "FileUtilSdk.h"
#include "FileUtilGenerator.h"
#include "FileUtilGeneratorAsync.h"
#include "ThreadPool.h"

FEXTERN CFileUtilGeneratorBase* CreateFactory(int async)
{
	if(async == 1)
	{
		return &CFileUtilGeneratorAsync::Instance();
	}
	else if(async == 0)
	{
		return &CFileUtilGenerator::Instance();
	}
	else
	{
		return nullptr;
	}
}

FEXTERN void WaitAsync()
{
	CThreadPool::Instance().JoinAll();
}